#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include <omp.h>

typedef struct { 
    int answer; // 0 for no answer, 1 ~ 9 is answer
    bool candidates[16];
} cell_t;

// program arguments helper functions
const char *get_option_string(const char *option_name, const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
static void show_help(const char *program_path);

// write to output file
void output_solution(int **sudoku, int grid_size);

// compute main function
void compute();

// update candidates and check if answer of this cell is found
void update_cell();

// update all candidates
void horizontal_update(cell_t **sudoku, int grid_size);

#endif