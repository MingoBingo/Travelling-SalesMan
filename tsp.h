#ifndef TSP_H
#define TSP_H

#include <stdio.h>
#include <stdlib.h>
#include "geometry.h"

void findMinRoom(Point A, Room *room, int numberOfRooms, 
    int *min_room_index, int *min_point_index, float *min_distance, float *totalCost);

void microTSP(Point *result, int *resIndex, int min_point_index, 
    Room room, int *visited, Point *last, float *totalCost);

Point *findPath(Point dockingStation, Room *room, int numberOfRooms, float *totalCost);

#endif