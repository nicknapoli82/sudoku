#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#define i8 int8_t
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
// TESTING
u64 count = 0;
u64 max_count = 0;
struct tile {
    u16 possible; // Bitfield of 9 bits
                  // Default is 0x3fe where number 1 is zero based 1 bit
    i8  absolute;
};

typedef struct tile_env {
    i8 x;
    i8 y;
    i8 number;
    struct tile *tile;
    struct tile (*grid)[9];
} tile_env;

tile_env *gen_tile_env(i8 x, i8 y, i8 number, struct tile *tile, struct tile (*grid)[9]) {
    tile_env* t_env = calloc(1, sizeof(tile_env));
    if (t_env == NULL)
	return NULL;
    t_env->x = x;
    t_env->y = y;
    t_env->number = number;
    t_env->tile = tile;
    t_env->grid = grid;
    return t_env;
}

// NOTE: func(function env, func_queue *)
typedef struct func_queue func_queue;

typedef struct node_func {
    i8(*func)(void*, func_queue*); 
    void* env;
    struct node_func* next;
} node_func;

typedef struct func_queue {
    node_func* head;
    node_func* tail;
} func_queue;


i8 enqueue(func_queue* self, i8 (*action)(void *, func_queue *), void *env);
i8 dequeue(func_queue* self);
i8 tile_resolved(void *, func_queue *);
i8 tile_update(void *, func_queue *);
i8 tile_resolveBroadcast(void *, func_queue *);
void print_grid(struct tile[][9]);
void OOM_error();

typedef struct solvers_env {
    struct tile (*grid)[9];    
}solvers_env;

// Whatever algorithms I want can go here
i8 solve_possibleElimination_immediateGrid(void *e, func_queue *queue);
// i8 solve_possibleElimination(void *, func_queue *);
i8(*sudoku_solvers[])(void *, func_queue *) = { solve_possibleElimination_immediateGrid };

int main(void) {
    // Function queue, to just pass around and use
    func_queue *fQueue = calloc(1, sizeof(func_queue));
    struct tile grid[9][9];
    
    if (fQueue == NULL)
        OOM_error();
    // Read in input. Consumes first 81 digits found 0 - 9
    // and plops them into the grid
    {
        char *line_buffer = NULL;
        size_t line_len = 0;
        i8 digits_found = 0;
        struct tile *grid_iter = (struct tile *)grid;
        while (getline(&line_buffer, &line_len, stdin) > 0 && digits_found < 81) {
            for (size_t i = 0; line_buffer[i] && digits_found < 81; i++) {
                if(isdigit(line_buffer[i])) {
                    grid_iter->absolute = line_buffer[i] - '0';
                    grid_iter->possible = grid_iter->absolute ? 0 : 0x3fe;
                    digits_found++;
                    grid_iter++;
                }
            }
        }
        free(line_buffer);
    }

    // Check all tiles and push tile_resolved onto queue
    // yes this is kind of redundant, but it gets the process
    // in motion
    {
        for (int y = 0; y < 9; y++)
            for (int x = 0; x < 9; x++)
                if (grid[y][x].absolute) {
		    tile_env* t_env = gen_tile_env(x, y, grid[y][x].absolute, &grid[y][x], grid);
                    if (t_env == NULL)
                        OOM_error();
                    if (enqueue(fQueue, tile_resolved, t_env) == 0) {
                        OOM_error();
                    }
                }
    }

    // Process queue until its empty
    while (1) {
        while (fQueue->head != NULL) {
            if (dequeue(fQueue) == 0) {
                // TODO: Add error reporting if necessary
                printf("Something bad happened!\n");
                exit(2);
            }
        }
	
        for (u64 i = 0; i < sizeof(sudoku_solvers) / sizeof(sudoku_solvers[0]); i++) {
	    solvers_env *env = malloc(sizeof(solvers_env));
	    env->grid = grid;
	    if(enqueue(fQueue, sudoku_solvers[i], env) == 0)
		OOM_error();
            if (dequeue(fQueue) == 0) {
                // TODO: Add error reporting if necessary
                printf("Something bad happened!\n");
                exit(2);
            }
        }

        if (fQueue->head == NULL)
            break;
    }
    
    print_grid(grid);
    printf("%lu\n", max_count);
    free(fQueue);
}

i8 enqueue(func_queue* self, i8 (*action)(void *, func_queue *), void *env) {
    count++;
    if(count > max_count)
	max_count = count;
    node_func* n = calloc(1, sizeof(node_func));
    if (n == NULL) return 0;
    n->func = action;
    n->env = env;
    if (self->head == NULL) {
        self->head = n;
        self->tail = n;
    }
    else {
        self->tail->next = n;
        self->tail = n;
    }
    return 1;
}

i8 dequeue(func_queue *self) {
    count--;
    i8 result = 1;
    if (self->head != NULL)
        result = self->head->func(self->head->env, self);
    if (self->head != NULL) {
        node_func *trash = self->head;
        self->head = self->head->next;
        free(trash->env);
        free(trash);
    }
    return result;
}

i8 tile_resolved(void *e, func_queue *queue) {
    func_queue *fq = queue;
    tile_env *env = e;
    struct tile *t = env->tile;
    t->absolute = env->number;
    t->possible = 0;
    tile_env *t_new = gen_tile_env(env->x, env->y, t->absolute, &env->grid[env->y][env->x], env->grid);
    if (t_new == NULL) OOM_error();
    if (enqueue(fq, tile_resolveBroadcast, t_new) == 0)
	OOM_error();    
    return 1;
}

i8 tile_update(void *e, func_queue *queue) {
    func_queue *fq = queue;
    tile_env *env = e;
    struct tile *t = env->tile;
    // EARLY EXIT NO WORK TO DO!!!
    if (t->absolute)
        return 1;
    if (t->possible & (1 << env->number))
	t->possible = t->possible ^ (1 << env->number);
    for (int i = 1; i < 10; i++) {
        if (!(t->possible ^ (1 << i))) {
	    tile_env *t_new = gen_tile_env(env->x, env->y, i, &env->grid[env->y][env->x], env->grid);
            if (t_new == NULL) OOM_error();
            if (enqueue(fq, tile_resolved, t_new) == 0)
                OOM_error();
            break;
        }
    }
    return 1;
}

i8 tile_resolveBroadcast(void *e, func_queue *queue) {
    func_queue *fq = queue;
    tile_env *env = e;
    i8 lower_y = env->y - (env->y % 3);
    i8 lower_x = env->x - (env->x % 3);
    i8 upper_y = lower_y + 3;
    i8 upper_x = lower_x + 3;
    for (i8 t_y = lower_y; t_y < upper_y; t_y++) {
        for (i8 t_x = lower_x; t_x < upper_x; t_x++) {
	    if (env->grid[t_y][t_x].absolute)
		continue;
	    tile_env *t_new = gen_tile_env(t_x, t_y, env->number, &env->grid[t_y][t_x], env->grid);
            if (t_new == NULL) OOM_error();
            if (enqueue(fq, tile_update, t_new) == 0)
                OOM_error();
        }
    }
    for (i8 t_y = 0; t_y < 9; t_y++) {
	if (env->grid[t_y][env->x].absolute)
	    continue;
        if (!(t_y >= lower_y && t_y < upper_y)) {
            tile_env *t_new = gen_tile_env(env->x, t_y, env->number, &env->grid[t_y][env->x], env->grid);
            if (t_new == NULL) OOM_error();
            if (enqueue(fq, tile_update, t_new) == 0)
                OOM_error();        
        }
            
    }
    for (i8 t_x = 0; t_x < 9; t_x++) {
	if (env->grid[env->y][t_x].absolute)
	    continue;
        if (!(t_x >= lower_x && t_x < upper_x)) {
	    tile_env *t_new = gen_tile_env(t_x, env->y, env->number, &env->grid[env->y][t_x], env->grid);
            if (t_new == NULL) OOM_error();
            if (enqueue(fq, tile_update, t_new) == 0)
                OOM_error();        
        }
            
    }
    return 1;
}


/**********************************************************************/
/* Algorithms that extend beyond simple column, row, box eliminations */
/* begin here. 							      */
/**********************************************************************/

// For each tile, check immediate grid and remove possible as necessary
// EG: If we find two tiles with only (1, 2) possible
//     then for all other tiles in that immediate grid we
//     can eliminate 1 and 2 as possible
i8 solve_possibleElimination_immediateGrid(void *e, func_queue *queue) {
    func_queue *fq = queue;
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;
    for (u8 y = 0; y < 9; y++) {
	for (u8 x = 0; x < 9; x++) {
	    // Skip unnecessary tiles
	    if (grid[y][x].absolute)
		continue;
	    i8 lower_y = y - (y % 3);
	    i8 lower_x = x - (x % 3);
	    i8 upper_y = lower_y + 3;
	    i8 upper_x = lower_x + 3;
	    // For every tile check immediate grid
	    // mask, if mask matches check if one bit remains based on mask for
	    // immediate grid, column, row
	    u16 tile_mask = grid[y][x].possible;
	    // Count bits in mask to know how many tiles are required
	    // to ensure we know what may be absolute
	    u8 tiles_needed = 0;
	    u8 tiles_found = 0;
	    for (u8 i = 1; i < 10; i++) {
		if (tile_mask & (1 << i))
		    tiles_needed++;
	    }

	    for (u8 y_seek = lower_y; y_seek < upper_y; y_seek++) {
		for (u8 x_seek = lower_x; x_seek < upper_x && tiles_found < tiles_needed; x_seek++) {
		    if (!grid[y_seek][x_seek].absolute
			&& !(tile_mask ^ grid[y_seek][x_seek].possible)) {
			tiles_found++;
		    }
		}
	    }

	    // If we have an equivalent number of tiles as to what is
	    // possible in the mask, then we can remove those numbers
	    // as possible in all other tiles in the immediate grid space
	    if (tiles_needed == tiles_found) {
		for (u8 y1 = lower_y; y1 < upper_y; y1++) {
		    for (u8 x1 = lower_x; x1 < upper_x; x1++) {
			// Only push tile updates if the bit is set
			// for each tile
			u16 check_mask = grid[y1][x1].possible & tile_mask;
			for (u8 i = 1; i < 10; i++) {
			    if (check_mask & (1 << i)) {
				tile_env *new_t = gen_tile_env(x1, y1, i, &grid[y1][x1], grid);
				if (new_t == NULL) OOM_error();
				if (enqueue(fq, tile_update, new_t) == 0) {
				    OOM_error();
				}
			    }
			}
		    }
		}
	    }
	}
    }
    return 1;
}

// For each tile, check immediate grid, columns and rows to see if we can
// resolve more tiles
// EG: If we find two tiles with only (1, 2) possible
//     and another with (1, 2, 3) then we know that tile (1, 2, 3)
//     has to be 3
i8 solve_possibleElimination(void *e, func_queue *queue) {
    func_queue *fq = queue;
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;
    for (u8 y = 0; y < 9; y++) {
	for (u8 x = 0; x < 9; x++) {
	    // Skip unnecessary tiles
	    if (grid[y][x].absolute)
		continue;
	    
	    // For every tile check immediate grid, column, and row for a matching
	    // mask, if mask matches check if one bit remains based on mask for
	    // immediate grid, column, row
	    u16 tile_mask = grid[y][x].possible;
	    // Count bits in mask to know how many tiles are required
	    // to ensure we know what may be absolute
	    u8 tiles_needed = 0;
	    u8 tiles_found = 1; // Set to 1 to include self
	    for (u8 i = 1; i < 10; i++) {
		if (tile_mask & (1 << i))
		    tiles_needed++;
	    }
	    i8 lower_y = y - (y % 3);
	    i8 lower_x = x - (x % 3);
	    i8 upper_y = lower_y + 3;
	    i8 upper_x = lower_x + 3;

	    // I think its easier to check the immediate grid first, because columns
	    // and rows are so easily defined
	    // Check immediate grid
	    for (u8 y_seek = lower_y; y_seek < upper_y; y_seek++) {
		for (u8 x_seek = lower_x; x_seek < upper_x && tiles_found < tiles_needed; x_seek++) {
		    if (!grid[y_seek][x_seek].absolute
			&& !(y_seek == y && x_seek == x)
			&& !(tile_mask ^ grid[y_seek][x_seek].possible)) {
			tiles_found++;
		    }
		}
	    }

	    // Check columns for matching mask
	    for (u8 y_seek = 0; y_seek < 9; y_seek++) {
		if (!grid[y_seek][x].absolute
		    && (y_seek < lower_y || y_seek >= upper_y)
		    && !(tile_mask ^ grid[y_seek][x].possible)) {
		    tiles_found++;
		}
	    }
	    // Check rows for matching mask
	    for (u8 x_seek = 0; x_seek < 9; x_seek++) {
		if (!grid[y][x_seek].absolute
		    && (x_seek < lower_x || x_seek >= upper_x)
		    && !(tile_mask ^ grid[y][x_seek].possible)) {
		    tiles_found++;
		}
	    }

	    if (tiles_needed == tiles_found) {
		// This is basically a repeat of the above loops for immediate grid, columns and rows
		// The only difference is we apply the mask, and check for a single remaining bit
		u16 remaining = 0;
		u8 number = 0;
		u8 resolve_y = 0;
		u8 resolve_x = 0;
		for (u8 y_seek = lower_y; y_seek < upper_y && remaining < 2; y_seek++) {
		    for (u8 x_seek = lower_x; x_seek < upper_x && remaining < 2; x_seek++) {
			if (!grid[y_seek][x_seek].absolute
			    && !(y_seek == y && x_seek == x)) {
			    u8 count = 0;
			    u16 extract = tile_mask ^ grid[y_seek][x_seek].possible;
			    for (u8 i = 1; i < 10 && remaining < 2; i++) {
				if (extract & (1 << i)) {
				    count++;
				    number = i;
				}
			    }
			    if (count == 1) {
				resolve_y = y_seek;
				resolve_x = x_seek;
				remaining++;
			    }
			}
		    }
		}
		for (u8 y_seek = 0; y_seek < 9 && remaining < 2; y_seek++) {
		    if (!grid[y_seek][x].absolute
			&& (y_seek < lower_y || y_seek >= upper_y)
			&& !(tile_mask ^ grid[y_seek][x].possible)) {
			u8 count = 0;
			u16 extract = tile_mask ^ grid[y_seek][x].possible;
			for (u8 i = 1; i < 10 && remaining < 2; i++) {
			    if (extract & (1 << i)) {
				count++;
				number = i;
			    }
			}
			if (count == 1) {
			    resolve_y = y_seek;
			    resolve_x = x;
			    remaining++;
			}
		    }
		}
		for (u8 x_seek = 0; x_seek < 9 && remaining < 2; x_seek++) {
		    if (!grid[y][x_seek].absolute
			&& (x_seek < lower_x && x_seek >= upper_x)
			&& !(tile_mask ^ grid[y][x_seek].possible)) {
			u8 count = 0;
			u16 extract = tile_mask ^ grid[y][x_seek].possible;
			for (u8 i = 1; i < 10 && remaining < 2; i++) {
			    if (extract & (1 << i)) {
				count++;
				number = i;
			    }
			}
			if (count == 1) {
			    resolve_y = y;
			    resolve_x = x_seek;
			    remaining++;
			}
		    }
		}

		if (remaining == 1) {
		    tile_env *t_new = gen_tile_env(resolve_x, resolve_y, number, &env->grid[resolve_y][resolve_x], env->grid);
		    enqueue(fq, tile_resolved, t_new);
		}
	    }

	}
    }
    return 1;
}

void print_grid(struct tile grid[][9]) {
    char *TOP_BOT = "▓▓▓▓▓▓▓▓▓▓▓▓▓\n";
    char *ROW = "▓%i%i%i▒%i%i%i▒%i%i%i▓\n";
    char *MIDDLE = "▓▒▒▒▒▒▒▒▒▒▒▒▓\n";

    printf("%s", TOP_BOT);
    printf(ROW, grid[0][0].absolute, grid[0][1].absolute, grid[0][2].absolute, 
           grid[0][3].absolute, grid[0][4].absolute, grid[0][5].absolute,
           grid[0][6].absolute, grid[0][7].absolute, grid[0][8].absolute);         
    printf(ROW, grid[1][0].absolute, grid[1][1].absolute, grid[1][2].absolute, 
           grid[1][3].absolute, grid[1][4].absolute, grid[1][5].absolute,
           grid[1][6].absolute, grid[1][7].absolute, grid[1][8].absolute);
    printf(ROW, grid[2][0].absolute, grid[2][1].absolute, grid[2][2].absolute, 
           grid[2][3].absolute, grid[2][4].absolute, grid[2][5].absolute,
           grid[2][6].absolute, grid[2][7].absolute, grid[2][8].absolute);             
    printf("%s", MIDDLE);
    printf(ROW, grid[3][0].absolute, grid[3][1].absolute, grid[3][2].absolute, 
           grid[3][3].absolute, grid[3][4].absolute, grid[3][5].absolute,
           grid[3][6].absolute, grid[3][7].absolute, grid[3][8].absolute);         
    printf(ROW, grid[4][0].absolute, grid[4][1].absolute, grid[4][2].absolute, 
           grid[4][3].absolute, grid[4][4].absolute, grid[4][5].absolute,
           grid[4][6].absolute, grid[4][7].absolute, grid[4][8].absolute);
    printf(ROW, grid[5][0].absolute, grid[5][1].absolute, grid[5][2].absolute, 
           grid[5][3].absolute, grid[5][4].absolute, grid[5][5].absolute,
           grid[5][6].absolute, grid[5][7].absolute, grid[5][8].absolute);             
    printf("%s", MIDDLE);
    printf(ROW, grid[6][0].absolute, grid[6][1].absolute, grid[6][2].absolute, 
           grid[6][3].absolute, grid[6][4].absolute, grid[6][5].absolute,
           grid[6][6].absolute, grid[6][7].absolute, grid[6][8].absolute);         
    printf(ROW, grid[7][0].absolute, grid[7][1].absolute, grid[7][2].absolute, 
           grid[7][3].absolute, grid[7][4].absolute, grid[7][5].absolute,
           grid[7][6].absolute, grid[7][7].absolute, grid[7][8].absolute);
    printf(ROW, grid[8][0].absolute, grid[8][1].absolute, grid[8][2].absolute, 
           grid[8][3].absolute, grid[8][4].absolute, grid[8][5].absolute,
           grid[8][6].absolute, grid[8][7].absolute, grid[8][8].absolute);             
    printf("%s", TOP_BOT);    
}

void OOM_error() {
    // Don't care about cleaning up memory
    // Something went wrong and just exit
    printf("Something went wrong. OOM possible\n");
    exit(1);
}
