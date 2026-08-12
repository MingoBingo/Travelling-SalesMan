# TSP vs Backtracking (C Implementation)

TSP (Travelling Salesman Problem) optimized with the Nearest Neighbour Algorithm, compared side-by-side with the absolute brute-force solution (Backtracking) to measure speed and accuracy.

*Note: All the mock data and visual representation functions (SVG generation) are provided by AI. My work focused entirely on the core logic, memory management, and algorithms.**

## What it does
The program calculates the optimal path for a robot navigating between multiple rooms. It demonstrates the classic computer science trade-off: 
* **Nearest Neighbour (Greedy):** Instant execution, but a slightly sub-optimal path (~10-15% margin of error).
* **Backtracking (Brute Force):** Finds the absolute perfect mathematical path, but requires massive computational time. The algorithm is optimized using Branch and Bound.

## How to run
Compile the C file using GCC (make sure to link the math library):
```bash
gcc main.c -o tsp_solver -lm