#include "sudoku.h"

#include <assert.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>

static int _argc;
static const char **_argv;

const char *get_option_string(const char *option_name, const char *default_value) {
    for (int i = _argc - 2; i >= 0; i -= 2)
        if (strcmp(_argv[i], option_name) == 0)
            return _argv[i + 1];
    return default_value;
}

int get_option_int(const char *option_name, int default_value) {
    for (int i = _argc - 2; i >= 0; i -= 2)
        if (strcmp(_argv[i], option_name) == 0)
            return atoi(_argv[i + 1]);
    return default_value;
}

float get_option_float(const char *option_name, float default_value) {
    for (int i = _argc - 2; i >= 0; i -= 2)
        if (strcmp(_argv[i], option_name) == 0)
            return (float)atof(_argv[i + 1]);
    return default_value;
}

static void show_help(const char *program_path) {
    printf("Usage: %s OPTIONS\n", program_path);
    printf("\n");
    printf("OPTIONS:\n");
    printf("\t-f <input_filename> (required)\n");
    printf("\t-n <num_of_threads> (required)\n");
    printf("\t-g <grid_size>\n");
}

void init_sudoku(FILE *input, int grid_size, int **sudoku) {
    int tmp;
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size - 1; j++) {
            fscanf(input, "%d ", &tmp);
            sudoku[i][j] = tmp;
        }
        fscanf(input, "%d\n", &tmp);
        sudoku[i][grid_size-1] = tmp;
    }
}

void output_solution(int **sudoku, int grid_size) {
    FILE *output_file = fopen("solution.txt", "w");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            fprintf(output_file, "%d ", sudoku[i][j]);
        }
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

void compute() {
    /*
    init all candidate(&num_blank, );
    while (blank) {
        horizontal update candidates;
        vertical update;
        block update;
    }
    */
}

int main(int argc, const char *argv[]) {
    using namespace std::chrono;
    // typedef std::chrono::high_resolution_clock Clock;
    // typedef std::chrono::duration<double> dsec;

    // auto init_start = Clock::now();
    // double init_time = 0;

    _argc = argc - 1;
    _argv = argv + 1;

    const char *input_filename = get_option_string("-f", NULL);
    int num_of_threads = get_option_int("-n", 1);
    int grid_size = get_option_int("-g", 9);

    int error = 0;

    if (input_filename == NULL) {
        printf("Error: You need to specify -f.\n");
        error = 1;
    }

    if (error) {
        show_help(argv[0]);
        return 1;
    }

    printf("Sudoku Grid Size: %d * %d\n", grid_size, grid_size);
    printf("Number of threads: %d\n", num_of_threads);
    printf("Input file: %s\n", input_filename);

    FILE *input = fopen(input_filename, "r");

    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return 1;
    }

    int **sudoku = (int **)malloc(grid_size * sizeof(int *));
    for (int i = 0; i < grid_size; i++) {
        sudoku[i] = (int *)malloc(grid_size * sizeof(int));
    }

    init_sudoku(input, grid_size, sudoku);

    // compute time starts
    compute();
    // compute time ends

    // Write to output file
    output_solution(sudoku, grid_size);

    return 0;
}

