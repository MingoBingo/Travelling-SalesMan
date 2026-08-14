#ifndef RANDOMTSP_H
#define RANDOMSTP_H

#include "geometry.h"
#include "backtracking.h"

void swapRooms(Room *room_A, Room *room_B);

Point *findRandom2OPTPath(Point dockinStation, Point currentPoint, Room *room, 
                            int numberOfRooms, float *totalCost);

void reverseOrderRooms(int i, int j, Room *room);

float DP(Point dockingStation, Room *room, int numberOfRooms, float **costMatrix, int **connectionMatrix, Point *randomPath);

//int bestPermutation(Point currentPoint, Room nextRoom);

#endif