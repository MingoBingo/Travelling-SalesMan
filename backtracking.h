#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include <stdio.h>
#include <stdlib.h>
#include "geometry.h"

extern const int cornerPermutations[24][4];
extern const int inOutPermutations[12][2];

int solutionBKT(int numberOfRooms, int roomsCompleted);

void findPathBkt(Point dockingStation, Point currentStep, float currentCost, float *minTotalCost, int roomsCompleted, Room *room, 
                int numberOfRooms, Point *currentPath, Point *bestPath, int *algorithmPlace, float *bktHistory, float **costInteriorMatrix);

#endif