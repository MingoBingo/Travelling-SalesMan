#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "geometry.h"
#include "tsp.h"
#include "backtracking.h"
#include "export.h"
#include "random_tsp.h"
#include "test_scenarios.h"

void calculateDistances(float **costInteriorMatrix, Room *room, int numberOfRooms, const int inOutPermutations[12][2])
{
    for(int i = 0; i < numberOfRooms; ++i)
    {
        for(int j = 0; j < 12; ++j)
        {
            costInteriorMatrix[i][j] = distance(room[i].corner[inOutPermutations[j][0]], room[i].corner[inOutPermutations[j][1]]);
        }
    }
}

int main()
{
    srand(time(NULL));

    // ========================================================
    // GENERAL SETTINGS
    // ========================================================
    // CHANGE HERE: 1 = Enabled, 0 = Disabled
    int RUN_BACKTRACKING = 0; 
    
    Point dockingStation = {0, 0};
    int numberOfRooms = N_CIRCLE_MAX;
    Room *room = calloc(numberOfRooms, sizeof(Room));
    loadScenario_Circle(room, numberOfRooms);
    // Allocate the rooms 
    
    int puncteNou = 2 * numberOfRooms + 2; 
    int puncteVechi = 4 * numberOfRooms + 2; 

    // Allocate variables for BKT (even if it doesn't run, they must exist for safety during free())
    Point *currentPathBkt = NULL;
    Point *bestPathBkt = NULL;
    Point *traseuBKT = NULL;
    float costBKT = 9999999.0;
    double timeBKT = 0.0;
    int algorithmPlace = 0;
    float bktHistory[1000] = {10000}; 

    float **costInteriorMatrix = malloc(sizeof(float *) * numberOfRooms);
    for(int cnt = 0; cnt < numberOfRooms; ++cnt)
    {
        costInteriorMatrix[cnt] = malloc(sizeof(float) * 12);
    }
    calculateDistances(costInteriorMatrix, room, numberOfRooms, inOutPermutations);

    // ========================================================
    // ROUND 1: BACKTRACKING (Conditional)
    // ========================================================
    if (RUN_BACKTRACKING)
    {
        for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 
        
        currentPathBkt = malloc(sizeof(Point) * (2 * numberOfRooms));
        bestPathBkt = malloc(sizeof(Point) * (2 * numberOfRooms));
        traseuBKT = malloc(sizeof(Point) * puncteNou);
        
        printf("Running Backtracking (Please be patient, searching for the absolute optimum)...\n");
        fflush(stdout); 
        
        clock_t start = clock();
        findPathBkt(dockingStation, dockingStation, 0.0, &costBKT, 0, room, numberOfRooms, currentPathBkt, bestPathBkt, &algorithmPlace, bktHistory, costInteriorMatrix);
        clock_t end = clock();
        timeBKT = (double)(end - start) / CLOCKS_PER_SEC;

        traseuBKT[0] = dockingStation;
        for(int i = 0; i < 2 * numberOfRooms; i++) traseuBKT[i + 1] = bestPathBkt[i];
        traseuBKT[puncteNou - 1] = dockingStation;

        exportToSVG("harta_1_bkt.svg", traseuBKT, puncteNou, room, numberOfRooms);
    }

    // ========================================================
    // ROUND 2: TSP (Standard Heuristic)
    // ========================================================
    for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 

    printf("Running Standard TSP...\n");
    fflush(stdout); 
    
    float costTSP = 0;
    clock_t startTSP = clock();
    Point *traseuTSP = findPath(dockingStation, room, numberOfRooms, &costTSP);
    clock_t endTSP = clock();
    double timeTSP = (double)(endTSP - startTSP) / CLOCKS_PER_SEC;

    exportToSVG("harta_2_tsp.svg", traseuTSP, puncteVechi, room, numberOfRooms);


    // ========================================================
    // ROUND 3: RANDOM 2-OPT (Your new algorithm)
    // ========================================================
    for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 

    printf("Running Random 2-OPT (with Dynamic Programming)...\n");
    fflush(stdout);
    
    float cost2OPT = 999999999.0;
    float costMIN2OPT =  999999999.0;

    Point *traseu2OPT = malloc(sizeof(Point) * (numberOfRooms * 2 + 2));

    double time2OPT;

    Point *traseuOptim = malloc(sizeof(Point) * (numberOfRooms * 2 + 2));

    for(int i = 0; i < 5; ++i)
    {
        cost2OPT = 999999999.0;
        clock_t start2OPT = clock();
        traseu2OPT = findRandom2OPTPath(dockingStation, dockingStation, room, numberOfRooms, &cost2OPT);
        clock_t end2OPT = clock();
        time2OPT = (double)(end2OPT - start2OPT) / CLOCKS_PER_SEC;

        if(cost2OPT < costMIN2OPT)
        {
            costMIN2OPT = cost2OPT;
            for(int contor = 0; contor < puncteNou; ++contor)
                traseuOptim[contor] = traseu2OPT[contor];
        }
        free(traseu2OPT);
    }
        
    
    exportToSVG("harta_3_random_2opt.svg", traseuOptim, puncteNou, room, numberOfRooms);


    // ========================================================
    // GRAPHIC EXPORT 
    // ========================================================
    if (RUN_BACKTRACKING) {
        exportChartToSVG(bktHistory, algorithmPlace, costTSP, costMIN2OPT); 
    }

    // ========================================================
    // DISPLAY ULTIMATE COMPARISON
    // ========================================================
    printf("\n========================================================\n");
    printf("  FINAL BENCHMARKS: Algorithm Showdown (%d Rooms)\n", numberOfRooms);
    printf("========================================================\n");
    
    if (RUN_BACKTRACKING) {
        printf("1. Backtracking : Cost = %10.2f | Time = %8.6f seconds\n", costBKT, timeBKT);
    } else {
        printf("1. Backtracking : [ DISABLED for %d rooms ]\n", numberOfRooms);
    }
    
    printf("2. Standard TSP : Cost = %10.2f | Time = %8.6f seconds\n", costTSP, timeTSP);
    printf("3. Random 2-OPT : Cost = %10.2f | Time = %8.6f seconds\n", costMIN2OPT, 5 * time2OPT);
    printf("========================================================\n\n");

    // --- MEMORY CLEANUP ---
    if (currentPathBkt) free(currentPathBkt); 
    if (bestPathBkt) free(bestPathBkt); 
    if (traseuBKT) free(traseuBKT);
    
    free(room); free(traseuTSP); free(traseuOptim); 
    
    for(int i = 0; i < numberOfRooms; ++i) free(costInteriorMatrix[i]);
    free(costInteriorMatrix);
    
    return 0;
}