#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include <omp.h>
#include <assert.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef struct { 
    int answer; // 0 for no answer, 1 ~ 9 is answer
    bool candidates[16];
} cell_t;

// program arguments helper functions
const char *get_option_string(const char *option_name, const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
static void show_help(const char *program_path);


//initialize sudoku base on input
void init_sudoku(FILE *input, int grid_size, int **sudoku);

// write to output file
void output_solution(int **sudoku, int grid_size);

// compute main function
void compute(int grid_size, cell_t **sudoku, int *num_blank);

// update candidates and check if answer of this cell is found
void update_cell();

// update cells in different ways
bool horizontal_update(cell_t **sudoku, int grid_size);
bool vertical_update(cell_t **sudoku, int grid_size);
bool block_update(cell_t **sudoku, int grid_size);

#endif