# C++ Path Optimizer

## Introduction
The C++ Path Optimizer is a program that solves the Travelling Salesman Problem (TSP) for a set of points in a 2D plane, aiming to find the shortest path that visits each point exactly once and returns to the original point. It employs the Ant Colony Optimization (ACO) algorithm to achieve this. This is particularly useful for logistics, robotics, and various other applications where optimal path planning is crucial.

## Prerequisites
C++ Compiler (e.g., g++)

## Installation

Clone the repository:

`git clone https://github.com/yourusername/Path-Optimizer.git`

Navigate into the project folder:

`cd Path-Optimizer`

Build the program with a single command:

`make pathopt`

## Command Line Interface
Run the program using the command line (NOTE: For 'ALGO TYPE' chose between MST, FAST_TSP, or OPTIMAL_TSP:

`./pathOpt 'ALGO TYPE' < input.txt > output.txt`

input.txt: A text file containing the 2D coordinates of points, one point per line (x and y separated by space), and the number of coordinates in the list line.

output.txt: The file where the optimized path will be saved, with the first line showing the total path distance, and the following lines listing the coordinate path.

## Sample Input File

The input.txt should look something like this:

`10`
`-735 594`
`652 -43`
`-871 -813`
256 950
-753 64
489 -686
-22 759
-899 777
517 -316
361 42`
