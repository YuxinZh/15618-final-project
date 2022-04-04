#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include <omp.h>

// program arguments helper functions
const char *get_option_string(const char *option_name, const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
static void show_help(const char *program_path);

void output_solution(int **sudoku, int grid_size);

#endif