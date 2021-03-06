#include "sudoku.h"
#include <stdint.h>
#include <inttypes.h>
#include <cmath>

typedef unsigned long long wide;

const int prefills = 8;

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

// Initialize sudoku with input file
//0 represent blank cell when initialize
void init_sudoku(FILE *input, int grid_size, cell_t sudoku[][16], int *num_blank) {
    int tmp;
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size - 1; j++) {
            fscanf(input, "%d ", &tmp);

            if(tmp == 0) 
                (*num_blank)++;

            sudoku[i][j].answer = tmp;
            
            memset(sudoku[i][j].candidates, true, sizeof(sudoku[i][j].candidates));
        }
        fscanf(input, "%d\n", &tmp);

        if(tmp == 0) 
            (*num_blank)++;

        sudoku[i][grid_size-1].answer = tmp;
        memset(sudoku[i][grid_size-1].candidates, true, sizeof(sudoku[i][grid_size-1].candidates));

    }
}

// output solution sudoku
void output_solution_backtrack(int sudoku[][16], int grid_size) {
    FILE *output_file = fopen("solution.txt", "w");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            fprintf(output_file, "%d ", sudoku[i][j]);
        }
        fprintf(output_file, "\n");    }
    fclose(output_file);
}

// update cells with answer_status
void update_cell(cell_t &cell, bool *answer_status, int grid_size, bool &filled) {
    for (int i = 0; i < grid_size; i++) {
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
}

// look at cells horizontally and update them
bool horizontal_update(cell_t sudoku[][16], int grid_size, bool &filled) {
    bool has_blank = false;
    int i;

    #pragma omp parallel for default(shared) private(i) schedule(dynamic)
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

// look at cells vertically and update them
bool vertical_update(cell_t sudoku[][16], int grid_size, bool &filled) {
    bool has_blank = false;
    int i;
    #pragma omp parallel for default(shared) private(i) schedule(dynamic)
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

        if (num_blank != 0)
            has_blank = true;
    }

    return has_blank;
}

// look at cells in a block and update them
bool block_update(cell_t sudoku[][16], int grid_size, bool &filled) {
    bool has_blank = false;
    int block_size = (int)sqrt(grid_size);
    int block_i;
    #pragma omp parallel for default(shared) private(block_i) schedule(dynamic)
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

// from hex character to int
int hex_to_int (char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    
    // printf("error in hex_to_int\n");
    return 0;
}

// fill sudoku with a fake_binary char array
void fill_sudoku(cell_t sudoku[][16], char (&fake_binary)[16*16], int grid_size, int num_blank) {
    int blank_index = 0;
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            if (sudoku[i][j].answer > 0)
                continue;
            
            // found a blank cell
            int find_place = hex_to_int(fake_binary[blank_index]);
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

// check sudoku too see if there is any violations
bool check_sudoku(cell_t sudoku[][16], int grid_size) {
    bool answer_status[16];
    
    //check columns
    for (int i = 0; i < grid_size; i++) {
        memset(answer_status, false, sizeof(answer_status));
        for (int j = 0; j < grid_size; j++) {
            if(answer_status[sudoku[j][i].answer] == true) {
                return false;
            } else {
                answer_status[sudoku[j][i].answer] = true;
            }
        }
    }

    //check rows
    for (int i = 0; i < grid_size; i++) {
        memset(answer_status, false, sizeof(answer_status));
        for (int j = 0; j < grid_size; j++) {
            if(answer_status[sudoku[i][j].answer] == true) {
                return false;
            } else {
                answer_status[sudoku[i][j].answer] = true;
            }
        }
    }

    //check blocks
    for (int block_i = 0; block_i < grid_size; block_i++) { // for each block
        memset(answer_status, false, sizeof(answer_status));
        int block_size = (int)sqrt(grid_size);
        int cell_x_start = (block_i / block_size) * block_size;
        int cell_y_start = (block_i % block_size) * block_size;
        for (int cell_x = cell_x_start; cell_x < cell_x_start + block_size; cell_x++) {
            for (int cell_y = cell_y_start; cell_y < cell_y_start + block_size; cell_y++) {
                if(answer_status[sudoku[cell_x][cell_y].answer] == true) {
                    return false;
                } else {
                    answer_status[sudoku[cell_x][cell_y].answer] = true;
                }
            }
        }
    }

    return true;
}

// check sudoku too see if there is any violations
bool check_sudoku(int sudoku[][16], int grid_size) {
    bool answer_status[16];
    
    //check columns
    for (int i = 0; i < grid_size; i++) {
        memset(answer_status, false, 16*sizeof(bool));
        for (int j = 0; j < grid_size; j++) {
            if (sudoku[j][i] == 0) 
                continue;

            if(answer_status[sudoku[j][i] - 1] == true) {
                return false;
            } else {
                answer_status[sudoku[j][i] - 1] = true;
            }
        }
    }

    //check rows
    for (int i = 0; i < grid_size; i++) {
        memset(answer_status, false, 16*sizeof(bool));
        for (int j = 0; j < grid_size; j++) {
            if (sudoku[i][j] == 0) 
                continue;

            if(answer_status[sudoku[i][j] - 1] == true) {
                return false;
            } else {
                answer_status[sudoku[i][j] - 1] = true;
            }
        }
    }

    //check blocks
    for (int block_i = 0; block_i < grid_size; block_i++) { // for each block
        memset(answer_status, false, 16*sizeof(bool));
        int block_size = (int)sqrt(grid_size);
        int cell_x_start = (block_i / block_size) * block_size;
        int cell_y_start = (block_i % block_size) * block_size;
        for (int cell_x = cell_x_start; cell_x < cell_x_start + block_size; cell_x++) {
            for (int cell_y = cell_y_start; cell_y < cell_y_start + block_size; cell_y++) {
                if (sudoku[cell_x][cell_y] == 0) 
                    continue;
                if(answer_status[sudoku[cell_x][cell_y] - 1] == true) {
                    return false;
                } else {
                    answer_status[sudoku[cell_x][cell_y] - 1] = true;
                }
            }
        }
    }

    return true;
}

// find the first few blanks and return the total number of combinations to fill
wide find_possibilities(int num_possibility[][16], cell_t sudoku[][16], int grid_size, int num_blank){
    wide iterations = 1;
    int count = 0;
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size; j++) {
            if(sudoku[i][j].answer != 0) {
                num_possibility[i][j] = 1; //store 1 when the number is fixed
            }
            else {
                count++;
                num_possibility[i][j] = 0;
                for(int k = 0; k < grid_size; k++) {
                    if(sudoku[i][j].candidates[k] == true) {
                        num_possibility[i][j]++;
                    }
                }

                iterations *= (wide)num_possibility[i][j];
                if (count == num_blank)
                    return iterations;
            }
        }
    }
    return iterations;

}

// find the next few blanks and return the total number of combinations to fill
int find_next_possibility(int* x, int* y, int grid_size, int num_possibility[][16]) {
    // move to next cell when (x,y) != (0,0)
    if((*x) != 0 || (*y) != 0) {
        if(*y == (grid_size-1)){
            *y = 0;
            (*x)++;
            if(*x == grid_size) return -1; //reach the end of num_possibility
        }
        else{
            (*y)++;
        }
    }

    while(num_possibility[*x][*y] == 1) {
        if(*y == (grid_size-1)){
            *y = 0;
            (*x)++;
            if(*x == grid_size) return -1; //reach the end of num_possibility
        }
        else{
            (*y)++;
        }
    }
    return num_possibility[*x][*y];
}

// convert int to hex character 
char int_to_hex (int i) {
    switch (i) {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'a';
        case 11:
            return 'b';
        case 12:
            return 'c';
        case 13:
            return 'd';
        case 14:
            return 'e';
        case 15:
            return 'f';
        default:
            printf("WRONG INPUT");
            return '0';
    }
}

// given a number, convert it to an array which can be used to fill sudoku
void find_fake_binary(wide iteration_count, char (&fake_binary)[16*16], int grid_size, int num_possibility[][16]){
    int x = 0;
    int y = 0;
    int possibility = find_next_possibility(&x, &y, grid_size, num_possibility);
    int i = 0;
    wide quotient = iteration_count;
    wide remainder;
    do {
        remainder = quotient % (wide)possibility;
        quotient = quotient / (wide)possibility;
        possibility = find_next_possibility(&x, &y, grid_size, num_possibility);
        fake_binary[i] = int_to_hex((int)remainder);
        i++;
    } while(quotient != 0);

    return;    
}

// check if the number exits in row
bool check_num(int num, bool* row) {
    if(row[num - 1] == 1) {
        return true;
    }
    return false;
}

// backtrack function to fill sudoku in a serialy manner
void backtrack(int x, int y, int grid_size, int sudoku[][16], bool rows[][16], bool cols[][16], bool boxes[][16], bool *solved) {
    int sqrt_grid = int(sqrt(grid_size));

    //change this to support 16
    if (x == grid_size) {
        *solved = true;
        return;
    }

    //change this to support 16
    int update_x = x + (y+1)/grid_size;
    int update_y = (y+1) % grid_size;

    if (sudoku[x][y] != 0) {
        backtrack(update_x, update_y, grid_size, sudoku, rows, cols, boxes, solved);
    }
    else {
        for(int i = 1; i < grid_size + 1; i++) {
            int box_id = x/sqrt_grid*sqrt_grid + y/sqrt_grid;
            if(check_num(i, rows[x])==false && check_num(i, cols[y])==false && check_num(i, boxes[box_id])==false){
                rows[x][i-1] = 1;
                cols[y][i-1] = 1;
                boxes[box_id][i-1] = 1;
                sudoku[x][y] = i; 

                backtrack(update_x, update_y, grid_size, sudoku, rows, cols, boxes, solved);

                if (*solved == false) {
                    rows[x][i-1] = 0;
                    cols[y][i-1] = 0;
                    boxes[box_id][i-1] = 0;
                    sudoku[x][y] = 0; 
                }
            }
        }
    }
}

// init sudoku for the use of backtracking
void init_sudoku_backtrack(int grid_size, int sudoku[][16], bool rows[][16], bool cols[][16], bool boxes[][16]) {
    int tmp;
    int sqrt_grid = int(sqrt(grid_size));
    
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size; j++) {
            tmp = sudoku[i][j];
            if(tmp != 0) {
                rows[i][tmp-1] = 1;
                cols[j][tmp-1] = 1;
                int box_id = i/sqrt_grid*sqrt_grid + j/sqrt_grid;
                boxes[box_id][tmp-1] = 1;
            }
        }
    }
}

// a backtrack wrapper function
bool do_backtrack(int grid_size, int sudoku[][16]) {

    bool rows[16][16] = {0};
    bool cols[16][16] = {0};
    bool boxes[16][16] = {0};

    init_sudoku_backtrack(grid_size, sudoku, rows, cols, boxes);

    bool solved = false;
    
    backtrack(0, 0, grid_size, sudoku, rows, cols, boxes, &solved);

    return solved;
}

// covert cell arrays to int arrays
void cells_to_ints(int grid_size, cell_t sudoku[][16], int sudoku_int[][16]) {
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {

            sudoku_int[i][j] = sudoku[i][j].answer;

        }
    }
}

// print sudoku
void print_sudoku (int grid_size, int sudoku[][16]) {
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {

            printf("%d ", sudoku[i][j]);

        }
        printf("\n");
    }

}

// main compute function to solve sudoku
void compute(int grid_size, cell_t sudoku[][16], int *num_blank, cell_t sudoku_answer[][16], unsigned int num_of_threads, int sudoku_backtrack_answer[][16]) {
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
            memcpy (sudoku_answer, sudoku, 16*16*sizeof(cell_t));
            return;
        }
        has_blank = vertical_update(sudoku, grid_size, filled);
        if (!has_blank) {
            printf("break by vertical\n");
            memcpy (sudoku_answer, sudoku, 16*16*sizeof(cell_t));
            return;
        }
        has_blank = block_update(sudoku, grid_size, filled);
        if (!has_blank) {
            printf("break by block\n");
            memcpy (sudoku_answer, sudoku, 16*16*sizeof(cell_t));
            return;
        }
    } while (has_blank && filled);
    printf("Has blanks after first round: %d\n", has_blank);
    /* Done first round of filling */

    int num_possibility[16][16];
    wide num_iterations = find_possibilities(num_possibility, sudoku, grid_size, prefills);
    printf("num_iterations: %llu\n", num_iterations);

    wide i;
    bool hit = false;
    #pragma omp parallel for default(shared) private(i) schedule(dynamic)
    for (i = 0; i < num_iterations; i++) {
        if (!hit) {

            cell_t local_sudoku[16][16];
            memcpy (local_sudoku, sudoku, 16*16*sizeof(cell_t));

            char fake_binary[16*16];          
            find_fake_binary(i, fake_binary, grid_size, num_possibility);
            fill_sudoku(local_sudoku, fake_binary, grid_size, prefills);
            
            // back track
            int local_backtrack_answer [16][16] = {0};
            cells_to_ints(grid_size, local_sudoku, local_backtrack_answer);
            if (check_sudoku(local_backtrack_answer, grid_size)) {

                if (do_backtrack(grid_size, local_backtrack_answer)) {
                    #pragma omp critical
                    {
                        hit = true;
                        memcpy(sudoku_backtrack_answer, local_backtrack_answer, 16*16*sizeof(int));
                    }
                    
                }
            }
        }
    }
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

    cell_t sudoku[16][16];
    cell_t sudoku_answer[16][16];
    int num_blank = 0;

    init_sudoku(input, grid_size, sudoku, &num_blank);

    /* Set the number of threads for the parallel region */
    omp_set_num_threads(num_of_threads);

    int sudoku_backtrack_answer[16][16] = {0};

    // compute time starts
    auto compute_start = Clock::now();
    double compute_time = 0;
    compute(grid_size, sudoku, &num_blank, sudoku_answer, num_of_threads, sudoku_backtrack_answer);
    compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();
    printf("Computation Time: %lf.\n", compute_time);

    // double check out answer
    if (check_sudoku(sudoku_backtrack_answer, grid_size))
        printf("solution valid.\n");
    else
        printf("error exit.\n");

    // Write to output file
    output_solution_backtrack(sudoku_backtrack_answer, grid_size);

    return 0;
}

