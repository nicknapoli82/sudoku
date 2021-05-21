#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#define i8 int8_t
#define u16 uint16_t
#define u32 uint32_t

void print_grid();
void process_grid();

i8 tile_absolute(void *);

struct tile {
    u16 possible; // Bitfield of 9 bits
    i8  absolute;
};

struct tile grid[9][9];

typedef struct node_func {
    i8(*func)(void*); 
    void* env;
} node_func;

typedef struct tile_absolute_env {
    i8 x;
    i8 y;
    i8 number;
} tile_absolute_env;

typedef struct func_queue {
    node_func func;
    node_func next;
} func_queue;

node_func *fQueue;

i8 enqueue();
i8 dequeue();

int main(void) {
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
		    grid_iter->possible = grid_iter->absolute ? 0 : 0x1ff;
		    digits_found++;
		    grid_iter++;
		}
	    }
	}
	free(line_buffer);
    }

    print_grid();
}

void process_grid() {
    for (u32 gI = 0; gI < 9; gI++) {
	for (u32 gY = 0; gY < 0; gY++) {
	    if (grid[gI][gY].absolute) {
		//TODO
	    }
	}
    }
}

// TODO
i8 tile_absolute(void *e) {
    tile_absolute_env *env = e;
    
}

i8 enqueue() {
    
}

i8 dequeue();

void print_grid() {
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
