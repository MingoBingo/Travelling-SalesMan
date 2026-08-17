# Travelling Salesman - Room Routing Optimizer

This project implements a complex variation of the Travelling Salesperson Problem (TSP) written in **C**. The program calculates the optimal path for a robot navigating between multiple rooms, taking into account entry/exit points (room corners) and internal traversal costs, ultimately returning to a docking station (`dockingStation`).

*Note: All the mock data and visual representation functions (SVG generation) were provided by AI. My work focused entirely on the core logic, memory management, and algorithm optimization.*

## 🚀 Main Features & The Classic Trade-off

This project demonstrates the classic computer science trade-off between execution speed and mathematical perfection by comparing multiple algorithms side-by-side:

*   **Standard TSP (Nearest Neighbour / Greedy):** Instant execution, but yields a slightly sub-optimal path (usually a ~10-15% margin of error).
*   **Backtracking (Brute Force with Branch & Bound):** Finds the absolute perfect mathematical path. The massive computational time is heavily optimized using Branch and Bound techniques to prune inefficient routes early. Ideal for small datasets (N <= 8).
*   **Multi-Start 2-OPT (Dynamic Programming):** An extremely fast heuristic algorithm, executed in multiple iterations with random initial states. It avoids local minima and finds excellent routes on large maps in a fraction of a second.
*   **Graphical Visualization (SVG):** The project automatically exports the calculated routes into `.svg` files, allowing precise visualization of the polygons and route intersections.

## 🛠️ Architecture and Modules

The project is modularized to separate the core calculation logic from the visualization:

*   `main.c` - The entry point, manages the main flow, and benchmarks the algorithms.
*   `backtracking.c` - Implementation of the exhaustive search with Branch and Bound pruning.
*   `tsp.c` & `random_tsp.c` - Implementations for DP and Multi-Start 2-OPT.
*   `geometry.c` - Utility functions for distances and coordinate manipulation.
*   `export.c` - The rendering engine that converts coordinates into `.svg` visual files.
*   `test_scenarios.h` - Suites of maps for geometric testing and validation (e.g., circles, grids, scattered clusters).

## ⚙️ Compilation and Execution

To compile the project, you need **GCC** installed. Open the terminal in the project directory and run (note the `-lm` flag required for the math library on some systems):

```bash
gcc main.c backtracking.c export.c geometry.c random_tsp.c tsp.c -o tsp_solver -lm