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
    // SETĂRI GENERALE
    // ========================================================
    // SCHIMBĂ AICI: 1 = Activat, 0 = Dezactivat
    int RUN_BACKTRACKING = 0; 
    
    Point dockingStation = {0, 0};
    int numberOfRooms = N_SINGLE;
    Room *room = calloc(numberOfRooms, sizeof(Room));
    loadScenario_Circle(room, numberOfRooms);
    // Alocam camerele 
    
    
    

    int puncteNou = 2 * numberOfRooms + 2; 
    int puncteVechi = 4 * numberOfRooms + 2; 

    // Alocare variabile pentru BKT (chiar dacă nu rulează, trebuie să existe pentru siguranța la free())
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
    // RUNDA 1: BACKTRACKING (Condiționat)
    // ========================================================
    if (RUN_BACKTRACKING)
    {
        for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 
        
        currentPathBkt = malloc(sizeof(Point) * (2 * numberOfRooms));
        bestPathBkt = malloc(sizeof(Point) * (2 * numberOfRooms));
        traseuBKT = malloc(sizeof(Point) * puncteNou);
        
        printf("Ruleaza Backtracking (Ai putina rabdare, cautam optimul absolut)...\n");
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
    // RUNDA 2: TSP (Euristica Standard)
    // ========================================================
    for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 

    printf("Ruleaza Standard TSP...\n");
    fflush(stdout); 
    
    float costTSP = 0;
    clock_t startTSP = clock();
    Point *traseuTSP = findPath(dockingStation, room, numberOfRooms, &costTSP);
    clock_t endTSP = clock();
    double timeTSP = (double)(endTSP - startTSP) / CLOCKS_PER_SEC;

    exportToSVG("harta_2_tsp.svg", traseuTSP, puncteVechi, room, numberOfRooms);


    // ========================================================
    // RUNDA 3: RANDOM 2-OPT (Noul tău algoritm)
    // ========================================================
    for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 

    printf("Ruleaza Random 2-OPT (cu Programare Dinamica)...\n");
    fflush(stdout);
    
    float cost2OPT = 999999999.0;
    clock_t start2OPT = clock();
    Point *traseu2OPT = findRandom2OPTPath(dockingStation, dockingStation, room, numberOfRooms, &cost2OPT);
    clock_t end2OPT = clock();
    double time2OPT = (double)(end2OPT - start2OPT) / CLOCKS_PER_SEC;

    exportToSVG("harta_3_random_2opt.svg", traseu2OPT, puncteNou, room, numberOfRooms);


    // ========================================================
    // EXPORT GRAFIC 
    // ========================================================
    if (RUN_BACKTRACKING) {
        exportChartToSVG(bktHistory, algorithmPlace, costTSP, cost2OPT); 
    }

    // ========================================================
    // AFIȘAREA COMPARAȚIEI SUPREME
    // ========================================================
    printf("\n========================================================\n");
    printf("  BENCHMARK FINALE: Confruntarea Algoritmilor (%d Camere)\n", numberOfRooms);
    printf("========================================================\n");
    
    if (RUN_BACKTRACKING) {
        printf("1. Backtracking : Cost = %10.2f | Timp = %8.6f secunde\n", costBKT, timeBKT);
    } else {
        printf("1. Backtracking : [ DEZACTIVAT pentru %d camere ]\n", numberOfRooms);
    }
    
    printf("2. Standard TSP : Cost = %10.2f | Timp = %8.6f secunde\n", costTSP, timeTSP);
    printf("3. Random 2-OPT : Cost = %10.2f | Timp = %8.6f secunde\n", cost2OPT, time2OPT);
    printf("========================================================\n\n");

    // --- CURĂȚENIA MEMORIEI ---
    if (currentPathBkt) free(currentPathBkt); 
    if (bestPathBkt) free(bestPathBkt); 
    if (traseuBKT) free(traseuBKT);
    
    free(room); free(traseuTSP); free(traseu2OPT);
    
    for(int i = 0; i < numberOfRooms; ++i) free(costInteriorMatrix[i]);
    free(costInteriorMatrix);
    
    return 0;
}