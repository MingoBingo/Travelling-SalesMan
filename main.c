#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "geometry.h"
#include "tsp.h"
#include "backtracking.h"
#include "export.h"
#include "random_tsp.h"

int main()
{
    srand(time(NULL));

    // 1. Definim datele de start
    int numberOfRooms = 5;
    Point dockingStation = {0, 0};
    
    // Alocam camerele 
    Room *room = calloc(numberOfRooms, sizeof(Room));
    
    // --- MOCK DATA: 5 CAMERE ---
    
    // R0: Nord-Est
    room[0].corner[0] = (Point){100, 100}; room[0].corner[1] = (Point){300, 100};
    room[0].corner[2] = (Point){300, 300}; room[0].corner[3] = (Point){100, 300};

    // R1: Nord-Vest
    room[1].corner[0] = (Point){-400, 200}; room[1].corner[1] = (Point){-200, 200};
    room[1].corner[2] = (Point){-200, 500}; room[1].corner[3] = (Point){-400, 500};

    // R2: Sud-Est
    room[2].corner[0] = (Point){500, -500}; room[2].corner[1] = (Point){800, -500};
    room[2].corner[2] = (Point){800, -800}; room[2].corner[3] = (Point){500, -800};

    // R3: Sud-Vest
    room[3].corner[0] = (Point){-300, -200}; room[3].corner[1] = (Point){-100, -200};
    room[3].corner[2] = (Point){-100, -400}; room[3].corner[3] = (Point){-300, -400};

    // R4: Departe în Est
    room[4].corner[0] = (Point){900, 100};  room[4].corner[1] = (Point){1200, 100};
    room[4].corner[2] = (Point){1200, 400}; room[4].corner[3] = (Point){900, 400};

    int totalPuncte = 4 * numberOfRooms + 2; 

    // ========================================================
    // RUNDA 1: TSP (Euristica ta initiala)
    // ========================================================
    printf("Ruleaza Standard TSP...\n");
    fflush(stdout); // Forteaza afisarea in consola imediat!
    
    float costTSP = 0;
    clock_t start = clock();
    Point *traseuTSP = findPath(dockingStation, room, numberOfRooms, &costTSP);
    clock_t end = clock();
    double timeTSP = (double)(end - start) / CLOCKS_PER_SEC;

    exportToSVG("harta_1_tsp.svg", traseuTSP, totalPuncte, room, numberOfRooms);


    // ========================================================
    // RUNDA 2: RANDOM 2-OPT (Noul tau algoritm)
    // ========================================================
    printf("Ruleaza Random 2-OPT...\n");
    fflush(stdout);
    
    float cost2OPT = 0;
    start = clock();
    Point *traseu2OPT = findRandom2OPTPath(dockingStation, dockingStation, room, numberOfRooms, &cost2OPT);
    end = clock();
    double time2OPT = (double)(end - start) / CLOCKS_PER_SEC;

    exportToSVG("harta_2_random_2opt.svg", traseu2OPT, totalPuncte, room, numberOfRooms);


    // ========================================================
    // RUNDA 3: BACKTRACKING (Optimul Absolut)
    // ========================================================
    for(int i = 0; i < numberOfRooms; i++) room[i].visited = 0; 
    
    Point *currentPathBkt = malloc(sizeof(Point) * (4 * numberOfRooms));
    Point *bestPathBkt = malloc(sizeof(Point) * (4 * numberOfRooms));
    float costBKT = 9999999999.0;
    int algorithmPlace = 0;
    float bktHistory[1000] = {10000}; 
    double timeBKT = 0.0;
    Point *traseuBKT = malloc(sizeof(Point) * totalPuncte);

    printf("Ruleaza Backtracking (Ai putina rabdare, poate dura cateva secunde)...\n");
    fflush(stdout); // Asta te asigura ca vezi mesajul pe ecran inainte sa inghete in calcule
    
    if (1) 
    {
        start = clock();
        findPathBkt(dockingStation, dockingStation, 0.0, &costBKT, 0, room, numberOfRooms, currentPathBkt, bestPathBkt, &algorithmPlace, bktHistory);
        end = clock();
        timeBKT = (double)(end - start) / CLOCKS_PER_SEC;

        traseuBKT[0] = dockingStation;
        for(int i = 0; i < 4 * numberOfRooms; i++) traseuBKT[i + 1] = bestPathBkt[i];
        traseuBKT[totalPuncte - 1] = dockingStation;

        exportToSVG("harta_3_bkt.svg", traseuBKT, totalPuncte, room, numberOfRooms);
        exportChartToSVG(bktHistory, algorithmPlace, costTSP, cost2OPT); 
    } 

    // ========================================================
    // AFIȘAREA COMPARAȚIEI SUPREME
    // ========================================================
    printf("\n========================================================\n");
    printf("  BENCHMARK FINALE: Confruntarea Algoritmilor (%d Camere)\n", numberOfRooms);
    printf("========================================================\n");
    printf("1. Standard TSP : Cost = %10.2f | Timp = %8.6f secunde\n", costTSP, timeTSP);
    printf("2. Random 2-OPT : Cost = %10.2f | Timp = %8.6f secunde\n", cost2OPT, time2OPT);
    printf("3. Backtracking : Cost = %10.2f | Timp = %8.6f secunde\n", costBKT, timeBKT);
    printf("========================================================\n\n");

    // --- CURĂȚENIA MEMORIEI ---
    free(currentPathBkt); free(bestPathBkt); free(traseuBKT);
    free(room); free(traseuTSP); free(traseu2OPT);
    
    return 0;
}