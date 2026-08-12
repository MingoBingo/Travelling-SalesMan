#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct 
{
    int x;
    int y;
}Point;

typedef struct 
{
    Point corner[4];
    int visited;
}Room;

const int cornerPermutations[24][4] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1},
    {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0},
    {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
    {3, 0, 1, 2}, {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 1, 2, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
};

float distance(Point A, Point B)
{
    return sqrt(pow((B.x - A.x), 2) + pow((B.y - A.y), 2));
}

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
        if((*min_distance) != 99999.0)
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
        min_dist = 99999.0;
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
        
        if(min_dist != 99999.0)
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

    float min_distance = 99999.0;
    int min_room_index, min_point_index;
    findMinRoom(dockingStation, room, numberOfRooms, &min_room_index,
        &min_point_index, &min_distance, totalCost);
    
    result[resIndex] = dockingStation;
    resIndex++;
    
    Point last;

    for(int i = 0; i < numberOfRooms; ++i)
    {
        min_distance = 99999.0;
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

            for(int i=0; i < numberOfRooms * 4; ++i)
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

void exportToSVG(Point *traseu, int totalPuncte, Room *rooms, int numberOfRooms) {
    FILE *f = fopen("harta_traseu.svg", "w");
    if (!f) {
        printf("Eroare la crearea fisierului SVG!\n");
        return;
    }

    // Calculăm "rama" imaginii (Bounding Box) pentru a ne asigura că încap toate camerele
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (int i = 0; i < totalPuncte; i++) {
        if (traseu[i].x < minX) minX = traseu[i].x;
        if (traseu[i].x > maxX) maxX = traseu[i].x;
        if (traseu[i].y < minY) minY = traseu[i].y;
        if (traseu[i].y > maxY) maxY = traseu[i].y;
    }
    
    int padding = 150; // Lăsăm puțin spațiu pe margini
    int width = maxX - minX + 2 * padding;
    int height = maxY - minY + 2 * padding;

    // Antetul fișierului SVG
    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' viewBox='%d %d %d %d' style='background-color: #f4f4f4;'>\n",
            minX - padding, minY - padding, width, height);

    // 1. Desenăm camerele (Poligoane albastre)
    for (int i = 0; i < numberOfRooms; i++) {
        fprintf(f, "<polygon points='%d,%d %d,%d %d,%d %d,%d' fill='lightblue' stroke='blue' stroke-width='4' opacity='0.6'/>\n",
                rooms[i].corner[0].x, rooms[i].corner[0].y,
                rooms[i].corner[1].x, rooms[i].corner[1].y,
                rooms[i].corner[2].x, rooms[i].corner[2].y,
                rooms[i].corner[3].x, rooms[i].corner[3].y);
    }

    // 2. Desenăm traseul (Linie roșie întreruptă)
    fprintf(f, "<polyline points='");
    for (int i = 0; i < totalPuncte; i++) {
        fprintf(f, "%d,%d ", traseu[i].x, traseu[i].y);
    }
    fprintf(f, "' fill='none' stroke='red' stroke-width='5' opacity='0.8' stroke-dasharray='10,10'/>\n");

    // 3. Desenăm fiecare pas și numărul lui
    for (int i = 1; i < totalPuncte - 1; i++) {
        fprintf(f, "<circle cx='%d' cy='%d' r='6' fill='black'/>\n", traseu[i].x, traseu[i].y);
        fprintf(f, "<text x='%d' y='%d' dx='10' dy='-10' font-size='20' fill='black' font-family='Arial'>%d</text>\n", 
                traseu[i].x, traseu[i].y, i);
    }

    // 4. Marcăm vizual Stația de Andocare (Verde și mare)
    fprintf(f, "<circle cx='0' cy='0' r='15' fill='green'/>\n");
    fprintf(f, "<text x='0' y='0' dx='20' dy='-20' font-size='28' fill='green' font-weight='bold' font-family='Arial'>START (0,0)</text>\n");

    // Închidem eticheta și fișierul
    fprintf(f, "</svg>\n");
    fclose(f);
    
    printf("\n--> Succes! Fisierul 'harta_traseu.svg' a fost generat in folderul curent.\n");
}

void exportChartToSVG(float *bktHistory, int historyCount, float tspCost) {
    FILE *f = fopen("grafic_performanta.svg", "w");
    if (!f) {
        printf("Eroare la crearea fisierului grafic SVG!\n");
        return;
    }

    int width = 1000;
    int height = 500;
    int padding = 80; // Am mărit marginea pentru mai mult spațiu sus și jos

    // 1. Căutăm minimul și maximul corect, explorând tot vectorul (rezistent la erori)
    float minCost = bktHistory[0];
    float maxCost = bktHistory[0];
    for (int i = 1; i < historyCount; i++) {
        if (bktHistory[i] < minCost) minCost = bktHistory[i];
        if (bktHistory[i] > maxCost) maxCost = bktHistory[i];
    }
    
    // Ne asigurăm că linia TSP intră în grafic chiar dacă e complet în afară
    if (tspCost < minCost) minCost = tspCost;
    if (tspCost > maxCost) maxCost = tspCost;

    // Adăugăm margini (15% sus ca să nu lovească titlul, 10% jos)
    float range = maxCost - minCost;
    if (range == 0) range = 1.0; // Evităm împărțirea la zero
    maxCost += range * 0.15;
    minCost -= range * 0.10;
    range = maxCost - minCost;

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 %d %d' style='background-color: #ffffff; font-family: Arial;'>\n", width, height);

    // 2. Desenăm axele
    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black' stroke-width='2'/>\n", padding, padding, padding, height - padding);
    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black' stroke-width='2'/>\n", padding, height - padding, width - padding, height - padding);

    // 3. Calculăm punctele pentru linia albastră (BKT)
    float dx = (float)(width - 2 * padding) / (historyCount > 1 ? historyCount - 1 : 1);
    
    fprintf(f, "<polyline points='");
    for (int i = 0; i < historyCount; i++) {
        float x = padding + i * dx;
        // Inversăm Y-ul pentru că în SVG coordonata 0 este sus
        float y = height - padding - ((bktHistory[i] - minCost) / range) * (height - 2 * padding);
        fprintf(f, "%.2f,%.2f ", x, y);
    }
    fprintf(f, "' fill='none' stroke='blue' stroke-width='4' opacity='0.7'/>\n");

    // Adăugăm punctele pe grafic și textele
    for (int i = 0; i < historyCount; i++) {
        float x = padding + i * dx;
        float y = height - padding - ((bktHistory[i] - minCost) / range) * (height - 2 * padding);
        fprintf(f, "<circle cx='%.2f' cy='%.2f' r='5' fill='blue'/>\n", x, y);
        // Afișăm valoarea doar pentru primul, ultimul și din când în când ca să nu aglomerăm vizual
        if (i == 0 || i == historyCount - 1 || i % 4 == 0) {
            fprintf(f, "<text x='%.2f' y='%.2f' dx='-15' dy='-12' font-size='12' fill='darkblue'>%d</text>\n", x, y, (int)bktHistory[i]);
        }
    }

    // 4. Desenăm linia ROȘIE pentru TSP
    float tspY = height - padding - ((tspCost - minCost) / range) * (height - 2 * padding);
    fprintf(f, "<line x1='%d' y1='%.2f' x2='%d' y2='%.2f' stroke='red' stroke-width='3' stroke-dasharray='10,10'/>\n", 
            padding, tspY, width - padding, tspY);
    fprintf(f, "<text x='%d' y='%.2f' dx='10' dy='-10' font-size='16' fill='red' font-weight='bold'>Cost TSP: %d</text>\n", 
            width - 250, tspY, (int)tspCost);

    // Titlu (mutat puțin mai sus)
    fprintf(f, "<text x='%d' y='%d' font-size='22' font-weight='bold' fill='black'>Evolutie BKT vs Limita TSP</text>\n", width / 2 - 150, padding - 30);

    fprintf(f, "</svg>\n");
    fclose(f);

    printf("\n--> Succes! Fisierul 'grafic_performanta.svg' a fost generat.\n");
}

int main()
{
    // 1. Definim datele de start
    int numberOfRooms = 4;
    Point dockingStation = {0, 0};
    
    // Alocam camerele 
    Room *room = calloc(numberOfRooms, sizeof(Room));
    
    // --- MOCK DATA: 4 CAMERE ---
    
    // Camera 0 (Aproape, Cadranul 1: Nord-Est)
    room[0].corner[0] = (Point){100, 100};
    room[0].corner[1] = (Point){300, 100};
    room[0].corner[2] = (Point){300, 300};
    room[0].corner[3] = (Point){100, 300};

    // Camera 1 (Distanță medie, Cadranul 2: Nord-Vest)
    room[1].corner[0] = (Point){-400, 200};
    room[1].corner[1] = (Point){-200, 200};
    room[1].corner[2] = (Point){-200, 500};
    room[1].corner[3] = (Point){-400, 500};

    // Camera 2 (Mai îndepărtată, Cadranul 4: Sud-Est)
    room[2].corner[0] = (Point){500, -500};
    room[2].corner[1] = (Point){800, -500};
    room[2].corner[2] = (Point){800, -800};
    room[2].corner[3] = (Point){500, -800};

    // Camera 3 (Aproape, Cadranul 3: Sud-Vest)
    room[3].corner[0] = (Point){-300, -200};
    room[3].corner[1] = (Point){-100, -200};
    room[3].corner[2] = (Point){-100, -400};
    room[3].corner[3] = (Point){-300, -400};

    // --- RULAREA ALGORITMULUI ---
    float totalCost = 0;
    Point *traseu = findPath(dockingStation, room, numberOfRooms, &totalCost);
    
    // --- AFIȘAREA REZULTATELOR ---
    // (4 colțuri per cameră) + start + final
    int totalPuncte = 4 * numberOfRooms + 2; 
    
    printf("--- Traseul optim calculat (Teste Complexe) ---\n");
    for(int i = 0; i < totalPuncte; ++i)
    {
        if (i == 0) {
            printf("START (Docking): (%4d, %4d)\n", traseu[i].x, traseu[i].y);
        } else if (i == totalPuncte - 1) {
            printf("\nFINAL (Docking): (%4d, %4d)\n", traseu[i].x, traseu[i].y);
        } else {
            // Spațiere vizuală pentru a evidenția grupurile de câte 4 colțuri
            if ((i - 1) % 4 == 0) {
                printf("\n -> Intrare Camera Noua\n");
            }
            printf("Pasul %2d:        (%4d, %4d)\n", i, traseu[i].x, traseu[i].y);
        }
    }
    printf("-----------------------------------------------\n");

    exportToSVG(traseu, totalPuncte, room, numberOfRooms);
    // --- CURĂȚENIA MEMORIEI ---

    for(int i = 0; i < numberOfRooms; i++) {
        room[i].visited = 0;
    }

    // Alocăm memorie doar pentru colțuri (BKT-ul nu pune statia de andocare in array)
    Point *currentPathBkt = malloc(sizeof(Point) * (4 * numberOfRooms));
    Point *bestPathBkt = malloc(sizeof(Point) * (4 * numberOfRooms));
    float minTotalCostBkt = 9999999999.0;

    // --- RULAREA BACKTRACKING-ULUI ---
    // Plecăm de la dockingStation, costul acumulat e 0, am terminat 0 camere
    int algorithmPlace = 0;
    float bktHistory[100] = {6500};
    findPathBkt(dockingStation, dockingStation, 0.0, &minTotalCostBkt, 0, room, numberOfRooms, currentPathBkt, bestPathBkt, &algorithmPlace, bktHistory);
    exportChartToSVG(bktHistory, algorithmPlace, totalCost);
    // --- AFIȘAREA COMPARAȚIEI SUPREME ---
    printf("\n===============================================\n");
    printf("REZULTATE FINALE: Nearest Neighbour vs BKT Optim\n");
    printf("===============================================\n");
    printf("Cost total TSP (Euristic): %f\n", totalCost);
    printf("Cost total BKT (Optim):    %f\n", minTotalCostBkt);
    printf("===============================================\n\n");

    free(currentPathBkt);
    free(bestPathBkt);
    free(room);
    free(traseu);
    
    return 0;
}