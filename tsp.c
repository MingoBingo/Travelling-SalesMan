#include "tsp.h"

void findMinRoom(Point A, Room *room, int numberOfRooms, 
    int *min_room_index, int *min_point_index, float *min_distance, float *totalCost)
    {
        for(int i = 0; i < numberOfRooms; ++i)
        {
            if(!room[i].visited)
            {
                for(int j = 0; j < 4; ++j)
                {
                    float k = distance(A, room[i].corner[j]);
                    if(k < (*min_distance))
                    {
                        (*min_distance) = k;
                        (*min_room_index) = i;
                        (*min_point_index) = j;
                    }
                }
            }
        }
        if((*min_distance) != 9999999.0)
            (*totalCost) += (*min_distance);
    }

void microTSP(Point *result, int *resIndex, int min_point_index, 
    Room room, int *visited, Point *last, float *totalCost)
{
    visited[min_point_index] = 1;
    float min_dist;
    int next_corner_index;

    for(int i = 0; i < 3; ++i)
    {
        min_dist = 9999999.0;
        next_corner_index = -1;
        for(int j = 0; j < 4; ++j)
        {
            if(!visited[j])
            {
                float k = distance(room.corner[min_point_index], room.corner[j]);

                if(k < min_dist)
                {
                    min_dist = k;
                    next_corner_index = j;
                }
            }
        }
        result[(*resIndex)].x = room.corner[next_corner_index].x;
        result[(*resIndex)].y = room.corner[next_corner_index].y;
        
        if(min_dist != 9999999.0)
            (*totalCost) += min_dist;
        
        (*resIndex)++;
        visited[next_corner_index] = 1;
        min_point_index = next_corner_index;
    }
    (*last).x = room.corner[min_point_index].x;
    (*last).y = room.corner[min_point_index].y;
    
}

Point *findPath(Point dockingStation, Room *room, int numberOfRooms, float *totalCost)
{
    Point *result = malloc(sizeof(Point) * (4 * numberOfRooms + 2));
    int resIndex = 0;

    float min_distance = 9999999.0;
    int min_room_index, min_point_index;
    findMinRoom(dockingStation, room, numberOfRooms, &min_room_index,
        &min_point_index, &min_distance, totalCost);
    
    result[resIndex] = dockingStation;
    resIndex++;
    
    Point last;

    for(int i = 0; i < numberOfRooms; ++i)
    {
        min_distance = 9999999.0;
        int visited[4] = {0};
        result[resIndex].x = room[min_room_index].corner[min_point_index].x;
        result[resIndex].y = room[min_room_index].corner[min_point_index].y;
        resIndex++;
        room[min_room_index].visited = 1;
        microTSP(result, &resIndex, min_point_index, room[min_room_index], visited, &last, totalCost);
        findMinRoom(last, room, numberOfRooms, &min_room_index, &min_point_index, &min_distance, totalCost);
    }
    result[resIndex].x = dockingStation.x;
    result[resIndex].y = dockingStation.y;

    (*totalCost) += distance(last, dockingStation);

    return result;
}