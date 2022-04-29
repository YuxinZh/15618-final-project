#include "sudoku.h"
#include <cmath>

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

            if(tmp == 0) 
                (*num_blank)++;

            sudoku[i][j].answer = tmp;
            
            // sudoku[i][j].candidates = {true};
            memset(sudoku[i][j].candidates, true, sizeof(sudoku[i][j].candidates));
        }
        fscanf(input, "%d\n", &tmp);

        if(tmp == 0) 
            (*num_blank)++;

        sudoku[i][grid_size-1].answer = tmp;
        // sudoku[i][grid_size-1].candidates = {true};
        memset(sudoku[i][grid_size-1].candidates, true, sizeof(sudoku[i][grid_size-1].candidates));

    }
}

void output_solution(cell_t **sudoku, int grid_size) {
    FILE *output_file = fopen("solution.txt", "w");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            fprintf(output_file, "%d ", sudoku[i][j].answer);
            // for(int k = 0; k < 16; k++) fprintf(output_file, "%d ", sudoku[i][j].candidates[k]);
            // fprintf(output_file, "\n");
        }
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

void update_cell(cell_t &cell, bool *answer_status, int grid_size, bool &filled) {
    for (int i = 0; i < grid_size; i++) {// might have bug for 9*9 ?
        cell.candidates[i] = cell.candidates[i] && answer_status[i];
    }
    
    int tmp_answer = 0;
    for (int i = 0; i < grid_size; i++) {
        if (cell.candidates[i] && (tmp_answer == 0)) {
            // meaning this is the first candidate in the list that is true
            tmp_answer = i + 1;
        } else if (cell.candidates[i] && (tmp_answer != 0)) {
            // meaning there are at least two remaining candidates
            tmp_answer = 0;
            break;
        }
    }
    if (tmp_answer > 0){
        cell.answer = tmp_answer;
        filled = true;
    }

    // return 0;
}

bool horizontal_update(cell_t **sudoku, int grid_size, bool &filled) {
    bool has_blank = false;
    int i;
    // dynamic openMP assign
    for (i = 0; i < grid_size; i++) { // for each horizontal line

        int num_blank = 0;
        int blank_index[16];
        bool answer_status[16];
        memset(answer_status, true, sizeof(answer_status));

        for (int j = 0; j < grid_size; j++) {
            if (sudoku[i][j].answer > 0) {
                answer_status[sudoku[i][j].answer-1] = false;
            } else {
                blank_index[num_blank] = j;
                num_blank++;
            }
        }

        for (int k = 0; k < num_blank; k++) {
            update_cell(sudoku[i][blank_index[k]], answer_status, grid_size, filled);
        }

        // can work with non-atomic instruction
        if (num_blank != 0)
            has_blank = true;
    }

    return has_blank;
}

bool vertical_update(cell_t **sudoku, int grid_size, bool &filled) {
    bool has_blank = false;
    int i;
    // dynamic openMP assign
    for (i = 0; i < grid_size; i++) { // for each vertical line

        int num_blank = 0;
        int blank_index[16];
        bool answer_status[16];
        memset(answer_status, true, sizeof(answer_status));
        
        for (int j = 0; j < grid_size; j++) {
            if (sudoku[j][i].answer > 0) {
                answer_status[sudoku[j][i].answer-1] = false;
            } else {
                blank_index[num_blank] = j;
                num_blank++;
            }
        }

        for (int k = 0; k < num_blank; k++) {
            update_cell(sudoku[blank_index[k]][i], answer_status, grid_size, filled);
        }

        // can work with non-atomic instruction
        if (num_blank != 0)
            has_blank = true;
    }

    return has_blank;
}

bool block_update(cell_t **sudoku, int grid_size, bool &filled) {
    bool has_blank = false;
    int block_size = (int)sqrt(grid_size);
    int block_i;
    // dynamic openMP assign
    for (block_i = 0; block_i < grid_size; block_i++) { // for each block

        int num_blank = 0;
        int blank_index[16];
        bool answer_status[16];
        memset(answer_status, true, sizeof(answer_status));

        int cell_x_start = (block_i / block_size) * block_size;
        int cell_y_start = (block_i % block_size) * block_size;
        int cell_num = 0;
        for (int cell_x = cell_x_start; cell_x < cell_x_start + block_size; cell_x++) {
            for (int cell_y = cell_y_start; cell_y < cell_y_start + block_size; cell_y++) {

                if (sudoku[cell_x][cell_y].answer > 0) {
                    answer_status[sudoku[cell_x][cell_y].answer-1] = false;
                } else {
                    blank_index[num_blank] = cell_num;
                    num_blank++;
                }
                cell_num++;
            }

        }

        for (int k = 0; k < num_blank; k++) {
            int abs_x = cell_x_start + (blank_index[k] / block_size);
            int abs_y = cell_y_start + (blank_index[k] % block_size);
            update_cell(sudoku[abs_x][abs_y], answer_status, grid_size, filled);
        }

        // can work with non-atomic instruction
        if (num_blank != 0)
            has_blank = true;
    }

    return has_blank;
}

void fill_sudoku(cell_t **sudoku, int *fake_binary, int grid_size, int num_blank) {
    int blank_index = 0;
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            if (sudoku[i][j].answer > 0)
                continue;
            
            // found a blank cell
            int find_place = fake_binary[blank_index];
            int current_place = -1;
            for (int candidate_i = 0; candidate_i < grid_size; candidate_i++) {
                if (sudoku[i][j].candidates[candidate_i] == true )
                    current_place++;

                if (current_place == find_place) {
                    sudoku[i][j].answer = candidate_i + 1;
                    break;
                }
            }
            blank_index++;
            if (blank_index >= num_blank)
                return;
        }
    }
}

void compute(int grid_size, cell_t **sudoku, int *num_blank) {
    if ((*num_blank) == 0) {
        printf("Input has no blank to fill.\n");
        return;
    }

    bool has_blank = true;
    bool filled;
    printf("Number of blanks in problem: %d \n", has_blank);
    do {
        filled = false;
        has_blank = horizontal_update(sudoku, grid_size, filled);
        if (!has_blank) {
            printf("break by horizontal\n");
            break;
        }
        has_blank = vertical_update(sudoku, grid_size, filled);
        if (!has_blank) {
            printf("break by vertical\n");
            break;
        }
        has_blank = block_update(sudoku, grid_size, filled);
        if (!has_blank) {
            printf("break by block\n");
            break;
        }
    } while (has_blank && filled);
    printf("Left blanks: %d\n", has_blank);
    /* Done first round of filling */

    // num_possibility [16 * 16];
    // fill this array;
    // found total number of iterations;

    // for (i to total_iterations)
    //      convert i to fake_binary
    //      fill sudoku with fake_binary
    //      check(temp_sudoku);
    //      if (success)
    //          break;
}

int main(int argc, const char *argv[]) {
    using namespace std::chrono;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> dsec;

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

    printf("Sudoku Size: %d * %d\n", grid_size, grid_size);
    printf("Number of threads: %d\n", num_of_threads);
    printf("Input file: %s\n\n", input_filename);

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
    // output_solution(sudoku, grid_size);

    // compute time starts
    auto compute_start = Clock::now();
    double compute_time = 0;
    compute(grid_size, sudoku, &num_blank);
    compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();
    printf("Computation Time: %lf.\n", compute_time);
    // compute time ends

    // Write to output file
    output_solution(sudoku, grid_size);

    return 0;
}

