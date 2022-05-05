#include "sudoku_backtrack.h"

static int _argc;
static const char **_argv;
// int final_sudoku[16][16] = {0};

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
void init_sudoku(FILE *input, int grid_size, int sudoku[16][16], bool rows[16][16], bool cols[16][16], bool boxes[16][16]) {
    int tmp;
    int sqrt_grid = int(sqrt(grid_size));
    for(int i = 0; i < grid_size; i++) {
        for(int j = 0; j < grid_size - 1; j++) {
            fscanf(input, "%d ", &tmp);
            sudoku[i][j] = tmp;
            //change this to support 16
            if(tmp != 0) {
                rows[i][tmp-1] = 1;
                cols[j][tmp-1] = 1;
                int box_id = i/sqrt_grid*sqrt_grid + j/sqrt_grid;
                boxes[box_id][tmp-1] = 1;
            }
        }
        fscanf(input, "%d\n", &tmp);
        sudoku[i][grid_size-1] = tmp;
        //change this to support 16
        if(tmp != 0) {
            rows[i][tmp-1] = 1;
            cols[grid_size-1][tmp-1] = 1;
            int box_id = i/sqrt_grid*sqrt_grid + (grid_size-1)/sqrt_grid;
            boxes[box_id][tmp-1] = 1;
        }
    }
}

void output_solution(int sudoku[16][16], int grid_size) {
    FILE *output_file = fopen("solution_backtrack.txt", "w");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            //fprintf(output_file, "%d ", sudoku[i][j]);
            printf("%d ", sudoku[i][j]);
        }
        //fprintf(output_file, "\n");
        printf("\n");
    }
    fclose(output_file);
}

//check if a number exists in row/col/box
int check_num(int num, bool row[16]) {
    if(row[num - 1] == 1) {
        return true;
    }
    return false;
}

std::pair<int, int> get_next_location(int grid_size, int sudoku[16][16]) {
    for (int row = 0; row < grid_size; row++)
        for (int col = 0; col < grid_size; col++)
            if (sudoku[row][col] == 0) {
                return std::make_pair(row, col);
            }
    return std::make_pair(-1, -1); //return -1 when sudoku is full
}

bool backtrack2(int grid_size, int sudoku[16][16], bool rows[16][16], bool cols[16][16], bool boxes[16][16]) {
    int sqrt_grid = int(sqrt(grid_size));

    std::pair<int, int> tmp = get_next_location(grid_size, sudoku);
    int x = tmp.first;
    int y = tmp.second;

    if(x == -1) {
        output_solution(sudoku, grid_size);
        exit(true);
        return true;
    }

    //try from number 1 to grid_size(9 or 16)
    for(int i = 1; i < grid_size + 1; i++) {
        int box_id = x/sqrt_grid*sqrt_grid + y/sqrt_grid;
        if(check_num(i, rows[x])==false && check_num(i, cols[y])==false && check_num(i, boxes[box_id])==false){
            rows[x][i-1] = 1;
            cols[y][i-1] = 1;
            boxes[box_id][i-1] = 1;
            sudoku[x][y] = i; 

            if (backtrack2(grid_size, sudoku, rows, cols, boxes)) {
                output_solution(sudoku, grid_size);
                exit(true);
                return true;
            }

            rows[x][i-1] = 0;
            cols[y][i-1] = 0;
            boxes[box_id][i-1] = 0;
            sudoku[x][y] = 0; 
        }
    }
    return false;
}

bool backtrack(int grid_size, int sudoku[16][16], bool rows[16][16], bool cols[16][16], bool boxes[16][16]) {
    int sqrt_grid = int(sqrt(grid_size));

    std::pair<int, int> tmp = get_next_location(grid_size, sudoku);
    int x = tmp.first;
    int y = tmp.second;

    if(x == -1) {
        output_solution(sudoku, grid_size);
        exit(true);
        return true;
    }

    //try from number 1 to grid_size(9 or 16)
   #pragma omp taskloop private(grid_size, sudoku, rows, cols, boxes) 
    for(int i = 1; i < grid_size + 1; i++) 
    {
        int box_id = x/sqrt_grid*sqrt_grid + y/sqrt_grid;
        if(check_num(i, rows[x])==false && check_num(i, cols[y])==false && check_num(i, boxes[box_id])==false){
            rows[x][i-1] = 1;
            cols[y][i-1] = 1;
            boxes[box_id][i-1] = 1;
            sudoku[x][y] = i; 

            if(backtrack2(grid_size, sudoku, rows, cols, boxes)) {
                output_solution(sudoku, grid_size);
                exit(true);
                return true;
            }

            rows[x][i-1] = 0;
            cols[y][i-1] = 0;
            boxes[box_id][i-1] = 0;
            sudoku[x][y] = 0; 
        }
    }
    return false;
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
    printf("Input file: %s\n", input_filename);

    FILE *input = fopen(input_filename, "r");

    if (!input) {
        printf("Unable to open file: %s.\n", input_filename);
        return 1;
    }

    bool rows[16][16] = {0};
    bool cols[16][16] = {0};
    bool boxes[16][16] = {0};
    int sudoku[16][16] = {0};

    init_sudoku(input, grid_size, sudoku, rows, cols, boxes);

    // compute time starts
    auto compute_start = Clock::now();
    double compute_time = 0;

    #pragma omp parallel
    backtrack(grid_size, sudoku, rows, cols, boxes);

    compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();
    printf("Computation Time: %lf.\n", compute_time);
    
    // compute time ends
    
    // Write to output file
    // output_solution(final_sudoku, grid_size);

    return 0;
}
