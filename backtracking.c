#include "backtracking.h"

const int cornerPermutations[24][4] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1},
    {0, 3, 1, 2}, {0, 3, 2, 1}, {1, 0, 2, 3}, {1, 0, 3, 2},
    {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0},
    {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0},
    {2, 3, 0, 1}, {2, 3, 1, 0}, {3, 0, 1, 2}, {3, 0, 2, 1},
    {3, 1, 0, 2}, {3, 1, 2, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
};

int solutionBKT(int numberOfRooms, int roomsCompleted)
{
    return (numberOfRooms == roomsCompleted);
}

void findPathBkt(Point dockingStation, Point currentStep, float currentCost, float *minTotalCost, int roomsCompleted, Room *room, 
                int numberOfRooms, Point *currentPath, Point *bestPath, int *algorithmPlace, float *bktHistory)
{
    if(currentCost >= (*minTotalCost))
        return;
    
    if(solutionBKT(numberOfRooms, roomsCompleted))
    {
        float costToHome = distance(currentStep, dockingStation);
        float totalCost = currentCost + costToHome;

        if(totalCost < (*minTotalCost))
        {
            (*minTotalCost) = totalCost;

            for(int i = 0; i < numberOfRooms * 4; ++i)
            {
                bestPath[i] = currentPath[i];
            }

            printf("%d) Cost Total: %f\n", (*algorithmPlace), totalCost);
            bktHistory[(*algorithmPlace)] = totalCost;
            (*algorithmPlace)++;
        }
        return;
    }
    for(int i = 0; i < numberOfRooms; ++i)
    {
        if(!room[i].visited)
        {
            room[i].visited = 1;
             for(int j = 0; j < 24; ++j)
            {
                float currentCostBranch = currentCost;
                currentCostBranch += distance(currentStep, room[i].corner[cornerPermutations[j][0]]);
                currentCostBranch += distance(room[i].corner[cornerPermutations[j][0]], room[i].corner[cornerPermutations[j][1]]);
                currentCostBranch += distance(room[i].corner[cornerPermutations[j][1]], room[i].corner[cornerPermutations[j][2]]);
                currentCostBranch += distance(room[i].corner[cornerPermutations[j][2]], room[i].corner[cornerPermutations[j][3]]);

                currentPath[roomsCompleted * 4] = room[i].corner[cornerPermutations[j][0]];
                currentPath[roomsCompleted * 4 + 1] = room[i].corner[cornerPermutations[j][1]];
                currentPath[roomsCompleted * 4 + 2] = room[i].corner[cornerPermutations[j][2]];
                currentPath[roomsCompleted * 4 + 3] = room[i].corner[cornerPermutations[j][3]];

                findPathBkt(dockingStation, room[i].corner[cornerPermutations[j][3]], currentCostBranch, minTotalCost, roomsCompleted + 1, room, numberOfRooms, 
                            currentPath, bestPath, algorithmPlace, bktHistory);
            }
            room[i].visited = 0;
        }
    }
}