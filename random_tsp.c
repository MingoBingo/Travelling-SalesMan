#include "random_tsp.h"

void swapRooms(Room *room_A, Room *room_B)
{
    Room aux = *room_A;
    *room_A = *room_B;
    *room_B = aux;
}

void reverseOrderRooms(int i, int j, Room *room, Room *aux, int numberOfRooms)
{
    for(int l = 0; l < numberOfRooms; ++l)
    {
        aux[l] = room[l];
    }
    int left = i;
    int right = j;

    while(left < right)
    {
        Room tmp = aux[left];
        aux[left] = aux[right];
        aux[right] = tmp;
        left++;
        right--;
    }
}

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

Point *findRandom2OPTPath(Point dockingStation, Point currentPoint, Room *room, int numberOfRooms, float *totalCost)
{
    Point *randomPath = malloc(sizeof(Point) * (numberOfRooms * 4 + 2));

    randomPath[0] = dockingStation;
    randomPath[numberOfRooms * 4 + 1] = dockingStation;

    for(int i = numberOfRooms-1; i >= 1; --i)
    {
        int k = rand() % (i + 1);
        swapRooms(&room[i], &room[k]);
    }

    for(int i = 0; i < numberOfRooms; ++i)
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

    int foundImprovement = 1;
    float auxCost;
    Room *aux = malloc(sizeof(Room) * numberOfRooms);

    while(foundImprovement)
    {
        foundImprovement = 0;
        
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
        
        for(int i = 0; i < numberOfRooms - 1; ++i)
        {
            for(int j = i + 1; j < numberOfRooms; ++j)
            {
                reverseOrderRooms(i, j, room, aux, numberOfRooms);

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
            }
            if(foundImprovement) break;
        }
    }
    free(aux);
    return randomPath;
}