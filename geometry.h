#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct 
{
    int x;
    int y;
}Point;

typedef struct 
{
    Point corner[4];
    int visited;
    int permutation;
}Room;

float distance(Point A, Point B);

#endif