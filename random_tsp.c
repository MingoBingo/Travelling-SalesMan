#include "random_tsp.h"

void swapRooms(Room *room_A, Room *room_B)
{
    Room aux = *room_A;
    *room_A = *room_B;
    *room_B = aux;
}

void reverseOrderRooms(int i, int j, Room *room)
{
    int left = i;
    int right = j;

    while(left < right)
    {
        Room tmp = room[left];
        room[left] = room[right];
        room[right] = tmp;
        left++;
        right--;
    }
}
/*
int bestPermutation(Point currentPoint, Room nextRoom)
{
    float minCost = 99999.0, permutationCost;
    int indexMinCostPermutation, originalPermutation;
    originalPermutation = nextRoom.permutation;
    for(int p = 0; p < 24; ++p)
    {
        permutationCost = 0.0;

        permutationCost += distance(currentPoint, nextRoom.corner[cornerPermutations[p][0]]);
        permutationCost += distance(nextRoom.corner[cornerPermutations[p][0]], nextRoom.corner[cornerPermutations[p][1]]);
        permutationCost += distance(nextRoom.corner[cornerPermutations[p][1]], nextRoom.corner[cornerPermutations[p][2]]);
        permutationCost += distance(nextRoom.corner[cornerPermutations[p][2]], nextRoom.corner[cornerPermutations[p][3]]);

        if(permutationCost < minCost)
        {
            minCost = permutationCost;
            indexMinCostPermutation = p;
        }
    }
    return indexMinCostPermutation;
}
*/

float DP(Point dockingStation, Room *room, int numberOfRooms, float **costMatrix, int **connectionMatrix, Point *randomPath)
{
    for(int p = 0; p < 12; ++p)
        {
            costMatrix[numberOfRooms - 1][p] = distance(room[numberOfRooms - 1].corner[inOutPermutations[p][0]], room[numberOfRooms - 1].corner[inOutPermutations[p][1]]) + 
                                                distance(dockingStation, room[numberOfRooms - 1].corner[inOutPermutations[p][1]]);
            connectionMatrix[numberOfRooms - 1][p] = -1;
        }

    for(int cnt = numberOfRooms - 2; cnt >= 0; --cnt)
        {
            for(int perm = 0; perm < 12; ++perm)
            {
                costMatrix[cnt][perm] = distance(room[cnt].corner[inOutPermutations[perm][0]], room[cnt].corner[inOutPermutations[perm][1]]);
                Point exit = room[cnt].corner[inOutPermutations[perm][1]];

                float minCost = 99999.0;
                int gudIndex = -1;

                for(int perm2 = 0; perm2 < 12; ++perm2)
                {
                    float branchCost = 0.0;
                    Point entrance = room[cnt + 1].corner[inOutPermutations[perm2][0]];
                    branchCost += distance(exit, entrance) + costMatrix[cnt + 1][perm2];

                    if(branchCost < minCost)
                    {
                        minCost = branchCost;
                        gudIndex = perm2;
                    }
                }
                
                costMatrix[cnt][perm] += minCost;
                connectionMatrix[cnt][perm] = gudIndex;   
            }
                
        }

        float minCost = 9999999.0;
        int firstIndex = -1;
        for(int l = 0; l < 12; ++l)
        {
            float length = distance(dockingStation, room[0].corner[inOutPermutations[l][0]]);

            length += costMatrix[0][l];

            if(length < minCost)
            {
                minCost = length;
                firstIndex = l;
            }

        }

        int currDoor = firstIndex;

        for(int plex = 0; plex < numberOfRooms; ++plex)
        {
            randomPath[plex * 2 + 1] = room[plex].corner[inOutPermutations[currDoor][0]];
            randomPath[plex * 2 + 2] = room[plex].corner[inOutPermutations[currDoor][1]];

            currDoor = connectionMatrix[plex][currDoor];
        }

        return minCost;
}

Point *findRandom2OPTPath(Point dockingStation, Point currentPoint, Room *room, int numberOfRooms, float *totalCost)
{
    Point *randomPath = malloc(sizeof(Point) * (numberOfRooms * 2 + 2));
    Point *bestPath = malloc(sizeof(Point) * (numberOfRooms * 2 + 2));

    float **costMatrix = malloc(sizeof(float *) * numberOfRooms);
    int **connectionMatrix = malloc(sizeof(int *) * numberOfRooms);

    for(int i = 0; i < numberOfRooms; ++i)
    {
        costMatrix[i] = malloc(sizeof(float) * 12);
        connectionMatrix[i] = malloc(sizeof(int) * 12);
    }

    randomPath[0] = dockingStation;
    randomPath[numberOfRooms * 2 + 1] = dockingStation;

    for(int i = numberOfRooms-1; i >= 1; --i)
    {
        int k = rand() % (i + 1);
        swapRooms(&room[i], &room[k]);
    }

    *totalCost = DP(dockingStation, room, numberOfRooms, costMatrix, connectionMatrix, randomPath);
    /*
    for(int i = numberOfRooms - 2; i >= 0; --i)
    {

        
        int j = rand() % 24;
        
        (*totalCost) += distance(currentPoint, room[i].corner[cornerPermutations[j][0]]);
        (*totalCost) += distance(room[i].corner[cornerPermutations[j][0]], room[i].corner[cornerPermutations[j][1]]);
        (*totalCost) += distance(room[i].corner[cornerPermutations[j][1]], room[i].corner[cornerPermutations[j][2]]);
        (*totalCost) += distance(room[i].corner[cornerPermutations[j][2]], room[i].corner[cornerPermutations[j][3]]);

        randomPath[(i * 4 + 1)] = room[i].corner[cornerPermutations[j][0]];
        randomPath[(i * 4 + 2)] = room[i].corner[cornerPermutations[j][1]];
        randomPath[(i * 4 + 3)] = room[i].corner[cornerPermutations[j][2]];
        randomPath[(i * 4 + 4)] = room[i].corner[cornerPermutations[j][3]];
        
        currentPoint = room[i].corner[cornerPermutations[j][3]];

        room[i].permutation = j;
        
    }

    (*totalCost) += distance(currentPoint, dockingStation);
    */

    int foundImprovement;

    if(numberOfRooms >= 2)
        foundImprovement = 1;
    else foundImprovement = 0;

    while(foundImprovement)
    {
        foundImprovement = 0;
        /*
        for(int i = 0; i < numberOfRooms; ++i)
        {   
            
            
            int originalPermutation = room[i].permutation;
            for(int p = 0; p < 24; ++p)
            {
                if(p == originalPermutation) continue;

                room[i].permutation = p;

                auxCost = 0.0;
                Point currPoint = dockingStation;

                for(int k = 0; k < numberOfRooms; ++k)
                {
                    int idx = room[k].permutation;

                    auxCost += distance(currPoint, room[k].corner[cornerPermutations[idx][0]]);
                    auxCost += distance(room[k].corner[cornerPermutations[idx][0]], room[k].corner[cornerPermutations[idx][1]]);
                    auxCost += distance(room[k].corner[cornerPermutations[idx][1]], room[k].corner[cornerPermutations[idx][2]]);
                    auxCost += distance(room[k].corner[cornerPermutations[idx][2]], room[k].corner[cornerPermutations[idx][3]]);

                    currPoint = room[k].corner[cornerPermutations[idx][3]];

                }
                
                auxCost += distance(currentPoint, dockingStation);

                if(auxCost < (*totalCost))
                {
                    (*totalCost) = auxCost;
                    
                    for(int i = 0; i < numberOfRooms; ++i)
                    {
                        int j = room[i].permutation;
                        randomPath[(i * 4 + 1)] = room[i].corner[cornerPermutations[j][0]];
                        randomPath[(i * 4 + 2)] = room[i].corner[cornerPermutations[j][1]];
                        randomPath[(i * 4 + 3)] = room[i].corner[cornerPermutations[j][2]];
                        randomPath[(i * 4 + 4)] = room[i].corner[cornerPermutations[j][3]];
                    }
                    originalPermutation = p;
                    foundImprovement = 1;
                }
                else room[i].permutation = originalPermutation;
            }
        }
        */
        for(int i = 0; i < numberOfRooms - 1; ++i)
        {
            for(int j = i + 1; j < numberOfRooms; ++j)
            {
                reverseOrderRooms(i, j, room);
                /*
                auxCost = 0.0;
                currentPoint = dockingStation;

                for(int k = 0; k < numberOfRooms; ++k)
                {
                    int idx = bestPermutation(currentPoint, aux[k]);
                    aux[k].permutation = idx;

                    auxCost += distance(currentPoint, aux[k].corner[cornerPermutations[idx][0]]);
                    auxCost += distance(aux[k].corner[cornerPermutations[idx][0]], aux[k].corner[cornerPermutations[idx][1]]);
                    auxCost += distance(aux[k].corner[cornerPermutations[idx][1]], aux[k].corner[cornerPermutations[idx][2]]);
                    auxCost += distance(aux[k].corner[cornerPermutations[idx][2]], aux[k].corner[cornerPermutations[idx][3]]);

                    currentPoint = aux[k].corner[cornerPermutations[idx][3]];
                }

                auxCost += distance(currentPoint, dockingStation);

                if(auxCost < (*totalCost))
                {
                    (*totalCost) = auxCost;
                    for(int i = 0; i < numberOfRooms; ++i)
                    {
                        room[i] = aux[i];
                    }
                    for(int i = 0; i < numberOfRooms; ++i)
                    {
                        int j = room[i].permutation;
                        randomPath[(i * 4 + 1)] = room[i].corner[cornerPermutations[j][0]];
                        randomPath[(i * 4 + 2)] = room[i].corner[cornerPermutations[j][1]];
                        randomPath[(i * 4 + 3)] = room[i].corner[cornerPermutations[j][2]];
                        randomPath[(i * 4 + 4)] = room[i].corner[cornerPermutations[j][3]];
                    }

                    foundImprovement = 1;
                    break;
                }
                */
                float minCost = DP(dockingStation, room, numberOfRooms, costMatrix, connectionMatrix, randomPath);
                
                if(minCost < (*totalCost))
                {
                    (*totalCost) = minCost;
                    foundImprovement = 1;

                    /*for(int klop = 0; klop < numberOfRooms * 2 + 2; ++klop)
                        bestPath[klop] = randomPath[klop];
                    */
                   break;
                }
                else
                {
                    reverseOrderRooms(i, j, room);
                }
            }
            if(foundImprovement) break;
        }
    }

    for(int i = 0; i < numberOfRooms; ++i)
    {
        free(connectionMatrix[i]);
        free(costMatrix[i]);
    }

    free(connectionMatrix);
    free(costMatrix);
    free(bestPath);
    return randomPath;
}