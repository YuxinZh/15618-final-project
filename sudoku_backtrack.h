#ifndef _SUDOKU_BACKTRACK_H_
#define _SUDOKU_BACKTRACK_H_

#include <omp.h>
#include <assert.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// program arguments helper functions
const char *get_option_string(const char *option_name, const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
static void show_help(const char *program_path);


//initialize sudoku base on input
void init_sudoku(FILE *input, int grid_size, int **sudoku, int **rows, int **cols, int **boxes);

//backtrack function to compute
void backtrack(int i, int j, int grid_size, int **sudoku, int **rows, int **cols, int **boxes);

//check if a number exists in row/col/box
bool check_num(int grid_size, int num, int* row);

// write to output file
void output_solution(int **sudoku, int grid_size);


#endif