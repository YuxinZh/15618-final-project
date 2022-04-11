# ParaSudoku
15-618 Parallel Computer Architecture and Programming Final Project

Yuxin Zhang, Alex Luo

## SUMMARY
We are going to implement an optimized Sudoku Solver on multi-core CPU platform. The Sudoku Solver will take an array of points with x axis, y axis and its value as input, and print a sudoku chart as output. 

## BACKGROUND
Sudoku is a puzzle in which players insert the numbers one to nine into a grid consisting of nine squares subdivided into a further nine smaller squares in such a way that every number appears once in each horizontal line, vertical line, and square. Since the scale of traditional Sudoku is small, we plan to build a project that support both 9x9 and 16x16 sudoku. In 16x16 sudoku, players will insert the 0 to F into a grid consisting of sixteen squares so that 0~F appears once in each horizontal line, vertical line, and square.

Since the program will need to try different numbers for each square, it will benefit from parallelism. Also, the placement of different squares can be done in parallel.

## THE CHALLENGE
Our challenge is preventing race condition from happening, and keep shared memory up to date all the time - if the shared memory doesn't have the latest sudoku, some threads may work on outdated sudoku, leading to incorrect result.  

Since each placement require information nearby, our program will high locality. The program will have high communication to computation ratio, and there will be divergent execution. The constraint would be the communication between different threads, since each time a placement is made, we need to broadcast the new chart to all threads. 

## RESOURCES
Our program will be utilizing the muticores on the GHC machines. Since our goal of this project is to come up with a good parallelism scheme, we will be writing our own naive Sudoku algorithm and parallelising it to get desired speedups. We would also need to find sample sudoku puzzles online as well as answers to test our program. 

## GOALS AND DELIVERABLES
PLAN TO ACHIEVE (100%)
* Capable of solving Sudoku with different sizes and different difficulty levels.

* Achieve close to Nx speedup with N cores given a large enough Sudoku puzzle.

* Display both the puzzle and solution in an easily readable format.

HOPE TO ACHIEVE (125%)

* Achieve a minimized overhead so that a Nx speedup is achieved with N cores no matter the puzzle size and difficulty level.

* Achieve a reasonably fast run time compared to other algorithms. 

MUST ACHIEVE: (75%)

* Speedup > 2x for all core counts

* Improvements on speedup with increasing core counts.

DELIVERABLES:

A short demo of the program that will show the input Sudoku puzzle and the solution found by the program in a neat manner.

Graphs showing the performance improvement versus serial implementation on different puzzle sizes and difficulty levels.

## PLATFORM CHOICE
We will be using OpenMP semantics to implement our parallelism algorithm. We have chosen this language because it is an address-sharing language and we can benefit from such sharing memory. 

## SCHEDULE
| Date | Plan | Person |
| --------- | ----------- | ----------- |
| 3/21 - 3/23 | Finish project proposal | Alex, Yuxin |
| 3/24 - 4/8 | Finish basic single core algorithm | Alex |
| 4/8 - 4/11 | Revisited algorithm and finish a different algorithm that uses backtracking| Yuxin |
| 4/11 - 4/15 | Finish milestone report | Alex, Yuxin |
| 4/15 - 4/27  | Parallel implementation on both algorithm and compare | Alex, Yuxin |
| 4/27 - 4/29 | Finish final report | Alex, Yuxin |
| 4/29 - 5/5 | Prepare for poster session | Alex, Yuxin |
