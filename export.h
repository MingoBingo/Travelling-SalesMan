#ifndef EXPORT_H
#define EXPORT_H

#include <stdio.h>
#include <stdlib.h>
#include "geometry.h"

void exportToSVG(const char *filename, Point *traseu, int totalPuncte, 
            Room *rooms, int numberOfRooms); // Draws the TSP route
void exportChartToSVG(float *bktHistory, int historyCount, float tspCost, float opt2Cost); // Draws a graph to see the performance of the TSP Algorithm compared to Backtracking

#endif