#include "sudoku.h"

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
    printf("\t-n <num_of_threads> (default is 1)\n");
    printf("\t-g <grid_size> (default is 9)\n");
}

//0 represent blank cell when initialize
void init_sudoku(FILE *input, int grid_size, cell_t **sudoku, int *num_blank) {
    int tmp;
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size - 1; j++) {
            fscanf(input, "%d ", &tmp);
            if(tmp == 0) (*num_blank)++;
            sudoku[i][j].answer = tmp;
            for(int k = 0; k < 16; k++) sudoku[i][j].candidates[k] = 0;
        }
        fscanf(input, "%d\n", &tmp);
        if(tmp == 0) (*num_blank)++;
        sudoku[i][grid_size-1].answer = tmp;
        for(int k = 0; k < 16; k++) sudoku[i][grid_size-1].candidates[k] = 0;
    }
}

void output_solution(cell_t **sudoku, int grid_size) {
    FILE *output_file = fopen("solution.txt", "w");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            fprintf(output_file, "%d ", sudoku[i][j].answer);
        }
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}


void compute(int grid_size, cell_t **sudoku, int *num_blank) {
    /*
    init all candidate(&num_blank, );
    while (blank) {
        horizontal update candidates;
        vertical update;
        block update;
    }
    */
    while ((*num_blank) != 0) {
        
    }
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

    cell_t **sudoku = (cell_t **)malloc(grid_size * sizeof(cell_t *));
    for (int i = 0; i < grid_size; i++) {
        sudoku[i] = (cell_t *)malloc(grid_size * sizeof(cell_t));
    }
    int num_blank = 0;

    init_sudoku(input, grid_size, sudoku, &num_blank);

    // compute time starts
    compute(grid_size, sudoku, &num_blank);
    // compute time ends

    // Write to output file
    output_solution(sudoku, grid_size);

    return 0;
}

