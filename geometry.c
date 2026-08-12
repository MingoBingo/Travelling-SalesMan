#include <math.h>
#include "geometry.h"

float distance(Point A, Point B)
{
    return sqrt(pow((B.x - A.x), 2) + pow((B.y - A.y), 2));
}
