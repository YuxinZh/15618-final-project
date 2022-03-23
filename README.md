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

## GOALS AND DELIVERABLES

## PLATFORM CHOICE

## SCHEDULE
