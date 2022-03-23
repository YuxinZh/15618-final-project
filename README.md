# ParaSudoku
15-618 Parallel Computer Architecture and Programming Final Project

Yuxin Zhang, Alex Luo

## SUMMARY
We are going to implement an optimized Sudoku Solver on multi-core CPU platform. The Sudoku Solver will take an array of points with x axis, y axis and its value as input, and print a sudoku chart as output. 

## BACKGROUND
Sudoku is a puzzle in which players insert the numbers one to nine into a grid consisting of nine squares subdivided into a further nine smaller squares in such a way that every number appears once in each horizontal line, vertical line, and square. Since the scale of traditional Sudoku is small, we plan to build a project that support both 9x9 and 16x16 sudoku. In 16x16 sudoku, players will insert the 0 to F into a grid consisting of sixteen squares so that 0~F appears once in each horizontal line, vertical line, and square.

Since the program will need to try different numbers for each square, it will benefit from parallelism. Also, the placement of different squares can be done in parallel.

## THE CHALLENGE
The main challenge is making our program support both 9x9 and 16x16 sudoku, and our program can benefit from parallelism in both mode. 

Another challenge is preventing race condition from happening, and keep shared memory up to date all the time - if the shared memory doesn't have the latest sudoku, some threads may work on outdated sudoku, leading to incorrect result.  

## RESOURCES
Our program will be utilizing the muticores on the GHC machines. Since our goal of this project is to come up with a good parallelism scheme, we will be writing our own naive Sudoku algorithm and parallelising it to get desired speedups. We would also need to find sample sudoku puzzles online as well as answers to test our program. 

## GOALS AND DELIVERABLES

## PLATFORM CHOICE

## SCHEDULE
| Date | Plan |
| --------- | ----------- |
| 3/21 - 3/23 | Finish project proposal |
| 3/24 - 4/8 | Finish basic algorithm for 9x9 Sudoku |
| 4/8 - 4/11 | Finish milestone report |
| 4/11 - 4/15 | Finish algorithm for 16x16 Sudoku | 
| 4/15 - 4/27  | Parallel implementation and testing | 
| 4/27 - 4/29 | Finish final report |
| 4/29 - 5/5 | Prepare for poster session |
