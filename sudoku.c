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
// Just tracks how large the queue grows
// thorugh execution
u64 count = 0;
u64 max_count = 0;

struct tile {
    u16 possible; // Bitfield of 9 bits
                  // Default is 0x3fe where number 1 is zero based 1 bit
    i8  absolute;
};

typedef struct tile_env {
    i8 y;
    i8 x;
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
i8 solve_possibleSinglehidden(void *, func_queue *);
i8 solve_possibleGroupOpen(void *, func_queue *);
i8 solve_numberLinedUpRow(void *e, func_queue *queue);
i8 solve_numberLinedUpColumn(void *e, func_queue *queue);
i8(*sudoku_solvers[])(void *, func_queue *) =
{ solve_possibleSinglehidden, solve_possibleGroupOpen,
  solve_numberLinedUpRow, solve_numberLinedUpColumn };

/*********************************************************/
/* Handles local scope allocations for whatever you want */
/* Frees them at end of function                         */
/* Just pair Allocs_push with Allocs_pop                 */
/*********************************************************/
typedef struct s_Points {
    void **points;
    u32 cursor;
    u32 length;
}s_Points;

typedef struct s_Allocs {
    s_Points **points;
    u32 cursor;
    u32 length;
}s_Allocs;

s_Allocs Allocs;

void Allocs_push(void) {
    if ( Allocs.cursor == Allocs.length ) {
        Allocs.points = realloc(Allocs.points, sizeof(void*) * (Allocs.length + 4));
        if ( Allocs.points == NULL )
            OOM_error();
        memset(Allocs.points + Allocs.length, 0, sizeof(void *) * 4);
        Allocs.length += 4;
    }
    if ( Allocs.points[0] != NULL )
        Allocs.cursor++;
}

void Allocs_pop() {
    if ( Allocs.points[Allocs.cursor] ) {
        s_Points *loc = Allocs.points[Allocs.cursor];
        for (u32 i = 0; i < loc->cursor; i++) {
            free(loc->points[i]);
            loc->points[i] = NULL;
        }
        free(loc->points);
        loc->cursor = 0;
        loc->length = 0;

        free(Allocs.points[Allocs.cursor]);
        Allocs.points[Allocs.cursor] = NULL;
    }
    if ( Allocs.cursor == 0 ) {
        free(Allocs.points);
        Allocs.points = NULL;
        Allocs.length = 0;
    }
    if ( Allocs.cursor > 0)
        Allocs.cursor--;
}

void* Allocs_mem_push(void *a) {
    if ( Allocs.points[Allocs.cursor] == NULL ) {
        Allocs.points[Allocs.cursor] = calloc(1, sizeof(s_Points));
    }
    s_Points *loc = Allocs.points[Allocs.cursor];
    if ( loc->cursor == loc->length ) {
        loc->points = realloc(loc->points, sizeof(void*) * (loc->length + 8));
        if ( loc->points == NULL )
            OOM_error();
        memset(loc->points + loc->length, 0, sizeof(void *) * 8);
        loc->length += 8;
    }
    loc->points[loc->cursor] = a;
    loc->cursor++;
    return a;
}

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
	    // If a solver has pushed anything onto the fQ
	    // Go ahead and run through the queue
	    if (fQueue->head)
		break;
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

/***********************************************************/
/* I'm entertaining the idea of just operating on the grid */
/* as through lists. This code does nothing right now for  */
/* the program. Its only here as reminder to self that I   */
/* have this opportunity. 				   */
/***********************************************************/

typedef struct T_List {
    u8 x;
    u8 y;
    struct tile* tile;
}T_List;

typedef struct list_tiles {
    u32 length;
    T_List *tiles;
}list_tiles;

// Creates a list of tiles on the heap and returns a pointer
// List of tiles generated are within the constraints of x and y
// and applies the function given to determine inclusion
// Max size of the list should be 9
list_tiles gen_tile_list(u8 lower_y, u8 upper_y, u8 lower_x, u8 upper_x,
			 void* discluding, struct tile (*grid)[9]) {
    T_List list[9] = {0};
    u8 list_cursor = 0;
    for (u8 y = lower_y; y < upper_y; y++) {
	for (u8 x = lower_x; x < upper_x; x++) {
	    if ( discluding != &grid[y][x] ) {
		list[list_cursor].y = y;
		list[list_cursor].x = x;
		list[list_cursor].tile = &grid[y][x];
		list_cursor++;
	    }
	}
    }

    T_List *result = NULL;
    if ( list_cursor > 0 ) {
	result = calloc(list_cursor, sizeof(T_List) * list_cursor);
	if ( result == NULL )
	    OOM_error();
	memcpy(result, list, sizeof(T_List) * list_cursor);
    }
    return (list_tiles){list_cursor, result};
}

list_tiles tile_lists_concat(list_tiles A, list_tiles B) {
    T_List *result = calloc(sizeof(T_List), A.length + B.length);
    if ( result == NULL )
	OOM_error();
    memcpy(result, A.tiles, sizeof(T_List) * A.length);
    memcpy(&result[A.length], B.tiles, sizeof(T_List) * B.length);
    return (list_tiles){A.length + B.length, result};
}

enum TILE_FILTERS { TF_TSN, TF_TN, TF_SN, TF_T, TF_S, TF_N };

list_tiles filter_list_tiles(list_tiles *tiles, struct tile *self, u32 number, enum TILE_FILTERS using, i8 (*f)()) {
    T_List list[9] = {0};
    u8 list_cursor = 0;
    for (u8 i = 0; i < tiles->length; i++) {
        i8 test = 0;
        switch (using) {
        case TF_TSN: { test = f(tiles->tiles[i], self, number); break; }
        case TF_TN: { test = f(tiles->tiles[i], number); break; }
        case TF_SN: { test = f(self, number); break; }
        case TF_T: { test = f(tiles->tiles[i]); break; }
        case TF_S: { test = f(self); break; }
        case TF_N: { test = f(number); break; }
        }
        if ( test ) {
            list[list_cursor] = tiles->tiles[i];
            list_cursor++;
        }
    }
    T_List *result = NULL;
    if ( list_cursor > 0 ) {
        result = calloc(list_cursor, sizeof(T_List) * list_cursor);
        if ( result == NULL )
            OOM_error();
        memcpy(result, list, sizeof(T_List) * list_cursor);
    }
    return (list_tiles){list_cursor, result};
}

i8 filter_not_absolute(T_List T) {
    return !T.tile->absolute;
}

i8 filter_possible_match(T_List T, u32 number) {
    return !(T.tile->possible ^ number) ? 1 : 0;
}

i8 filter_possible_NotExact(T_List T, u32 number) {
    return T.tile->possible ^ number ? 1 : 0;
}

i8 filter_is_possible(T_List T, u32 number) {
    return T.tile->possible & (1 << number) ? 1 : 0;
}

i8 filter_contains_onePossible(T_List T, u32 number) {
    return T.tile->possible & number ? 1 : 0;
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


// For each tile, check immediate grid, columns and rows to see if we can
// resolve more tiles
// EG: If we find two tiles with only (1, 2) possible
//     and another with (1, 2, 3) then we know we can
//     eliminate both 1 and 2 from that additional tile
i8 solve_possibleSinglehidden(void *e, func_queue *queue) {
    Allocs_push();
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;

    for (u8 y = 0; y < 9; y++) {
	for (u8 x = 0; x < 9; x++) {
	    if ( grid[y][x].absolute )
		continue;
	    list_tiles box_list = gen_tile_list(y - (y % 3), y - (y % 3) + 3, x - (x % 3), x - (x % 3) + 3, &grid[y][x], grid);
	    Allocs_mem_push(box_list.tiles);
	    box_list = filter_list_tiles(&box_list, NULL, 0, TF_T, filter_not_absolute);
	    Allocs_mem_push(box_list.tiles);
	    for (u8 i = 1; i < 10; i++) {
		if ( grid[y][x].possible & (1 << i) ) {
		    list_tiles possible = filter_list_tiles(&box_list, NULL, i, TF_TN, filter_is_possible);
		    Allocs_mem_push(possible.tiles);
		    if ( possible.length == 0 ) {
			tile_env *new_t = gen_tile_env(x, y, i, &grid[y][x], grid);
			if (new_t == NULL) OOM_error();
			if (enqueue(queue, tile_resolved, new_t) == 0) {
			    OOM_error();
			}			
			break;
		    }
		}
	    }
	}
    }
    Allocs_pop();
    return 1;
}

// Checks possible numbers in each tile
// If that tiles numbers possible can be singled out
// in its immediate grid space, we know it
// is absolutely that number
i8 solve_possibleGroupOpen(void *e, func_queue *queue) {
    Allocs_push();
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;
    for (u8 y = 0; y < 9; y++) {
	for(u8 x = 0; x < 9; x++) {
	    if (grid[y][x].absolute)
		continue;
	    list_tiles box_list = gen_tile_list(y - (y % 3), y - (y % 3) + 3, x - (x % 3), x - (x % 3) + 3, NULL, grid);
	    Allocs_mem_push(box_list.tiles);
	    list_tiles box_match = filter_list_tiles(&box_list, NULL, grid[y][x].possible, TF_TN, filter_possible_match);
	    Allocs_mem_push(box_match.tiles);

	    list_tiles col_list = gen_tile_list(0, 9, x, x + 1, NULL, grid);
	    Allocs_mem_push(col_list.tiles);
	    list_tiles col_match = filter_list_tiles(&col_list, NULL, grid[y][x].possible, TF_TN, filter_possible_match);
	    Allocs_mem_push(col_match.tiles);

	    list_tiles row_list = gen_tile_list(y, y + 1, 0, 9, NULL, grid);
	    Allocs_mem_push(row_list.tiles);
	    list_tiles row_match = filter_list_tiles(&row_list, NULL, grid[y][x].possible, TF_TN, filter_possible_match);
	    Allocs_mem_push(row_match.tiles);
	    // Count up how many are possible
	    u8 tiles_needed = 0;
	    for (u8 i = 1; i < 10; i++) {
		if (grid[y][x].possible & (1 << i))
		    tiles_needed++;
	    }

	    list_tiles box = {0, 0}, col = {0, 0}, row = {0, 0};
	    if ( box_match.length == tiles_needed ) {
		box = filter_list_tiles(&box_list, NULL, grid[y][x].possible, TF_TN, filter_contains_onePossible);
		Allocs_mem_push(box.tiles);
		box = filter_list_tiles(&box, NULL, grid[y][x].possible, TF_TN, filter_possible_NotExact);
		Allocs_mem_push(box.tiles);
	    }
	    if ( col_match.length == tiles_needed ) {
		col = filter_list_tiles(&col_list, NULL, grid[y][x].possible, TF_TN, filter_contains_onePossible);
		Allocs_mem_push(col.tiles);
		col = filter_list_tiles(&col, NULL, grid[y][x].possible, TF_TN, filter_possible_NotExact);
		Allocs_mem_push(col.tiles);
	    }
	    if ( row_match.length == tiles_needed ) {
		row = filter_list_tiles(&row_list, NULL, grid[y][x].possible, TF_TN, filter_contains_onePossible);
		Allocs_mem_push(row.tiles);
		row = filter_list_tiles(&row, NULL, grid[y][x].possible, TF_TN, filter_possible_NotExact);
		Allocs_mem_push(row.tiles);
	    }

	    list_tiles result = tile_lists_concat(box, col);
	    Allocs_mem_push(result.tiles);
	    result = tile_lists_concat(result, row);
	    Allocs_mem_push(result.tiles);	    
	    
	    for (u32 t= 0; t < result.length; t++ ) {
		for (u8 i = 1; i < 10; i ++) {
		    if ((result.tiles[t].tile->possible & grid[y][x].possible) & (1 << i)) {
			tile_env *new_t = gen_tile_env(result.tiles[t].x, result.tiles[t].y, i, result.tiles[t].tile, grid);
			if (new_t == NULL) OOM_error();
			if (enqueue(queue, tile_update, new_t) == 0) {
			    OOM_error();
			}			
		    }
		}
	    }
	}
    }
    Allocs_pop();
    return 1;
}

// For each possible number each tile
// Check the immediate 3x3 grid and the full rows
// for a match.
// If the number exists within the immediat grid row
// but not elsewhere in the immediate grid
i8 solve_numberLinedUpRow(void *e, func_queue *queue) {
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;

    for (u8 y = 0; y < 9; y++) {
	for (u8 x = 0; x < 9; x++) {
	    if ( grid[y][x].absolute )
		continue;
	    u8 lower_y = y - (y % 3);
	    u8 upper_y = lower_y + 3;
	    u8 lower_x = x - (x % 3);
	    u8 upper_x = lower_x + 3;	    

	    // we can eliminate all possibilities in the rest of the row
	    for (u8 i = 1; i < 10; i++) {
		u8 imm_flag_row = 0;
		u8 imm_flag_notRow = 0;
		if ( grid[y][x].possible & (1 << i) ) {
		    // Look for a match in the same row immediate grid
		    for (u8 look = lower_x; look < upper_x; look++) {
			if ( look != x
			     && !grid[y][look].absolute
			     && grid[y][look].possible & (1 << i) ) {
			    imm_flag_row = 1;
			}
		    }
		    // Look for no matches in separate rows
		    // within the immediate grid
		    for (u8 y1 = lower_y; y1 < upper_y && !imm_flag_notRow; y1++) {
			if ( y1 == y )
			    continue;
			for (u8 x1 = lower_x; x1 < upper_x && !imm_flag_notRow; x1++) {
			    if ( !grid[y1][x1].absolute
				 && grid[y1][x1].possible & (1 << i) ) {
				imm_flag_notRow = 1;
			    }
			}
		    }
		}
		if ( imm_flag_row && !imm_flag_notRow ) {
		    for (u8 x1 = 0; x1 < 9; x1++) {
			if ( !grid[y][x1].absolute
			     && (x1 < lower_x || x1 >= upper_x)
			     && grid[y][x1].possible & (1 << i) ) {
			    tile_env *new_t = gen_tile_env(x1, y, i, &grid[y][x1], grid);
			    if ( new_t == NULL )
				OOM_error();
			    if ( enqueue(queue, tile_update, new_t) == 0 )
				OOM_error();			    
			}
		    }
		}
	    }
	}
    }
    
    return 1;
}

// For each possible number each tile
// Check the immediate 3x3 grid and the full column
// for a match.
// If the number exists within the immediat grid column
// but not elsewhere in the immediate grid
// we can eliminate all possibilities in the rest of the column
i8 solve_numberLinedUpColumn(void *e, func_queue *queue) {
    solvers_env *env = e;
    struct tile (*grid)[9] = env->grid;

    for (u8 y = 0; y < 9; y++) {
	for (u8 x = 0; x < 9; x++) {
	    if ( grid[y][x].absolute )
		continue;
	    u8 lower_y = y - (y % 3);
	    u8 upper_y = lower_y + 3;
	    u8 lower_x = x - (x % 3);
	    u8 upper_x = lower_x + 3;	    

	    for (u8 i = 1; i < 10; i++) {
		u8 imm_flag_row = 0;
		u8 imm_flag_notRow = 0;
		if ( grid[y][x].possible & (1 << i) ) {
		    // Look for a match in the same column immediate grid
		    for (u8 look = lower_y; look < upper_y; look++) {
			if ( look != y
			     && !grid[look][x].absolute
			     && grid[look][x].possible & (1 << i) ) {
			    imm_flag_row = 1;
			}
		    }
		    // Look for no matches in separate columns
		    // within the immediate grid
		    for (u8 y1 = lower_y; y1 < upper_y && !imm_flag_notRow; y1++) {
			for (u8 x1 = lower_x; x1 < upper_x && !imm_flag_notRow; x1++) {
			    if ( x1 == x )
				continue;
			    if ( !grid[y1][x1].absolute
				 && grid[y1][x1].possible & (1 << i) ) {
				imm_flag_notRow = 1;
			    }
			}
		    }
		}
		if ( imm_flag_row && !imm_flag_notRow ) {
		    for (u8 y1 = 0; y1 < 9; y1++) {
			if ( !grid[y1][x].absolute
			     && (y1 < lower_y || y1 >= upper_y)
			     && grid[y1][x].possible & (1 << i) ) {
			    tile_env *new_t = gen_tile_env(x, y1, i, &grid[y1][x], grid);
			    if ( new_t == NULL )
				OOM_error();
			    if ( enqueue(queue, tile_update, new_t) == 0 )
				OOM_error();			    
			}
		    }
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
