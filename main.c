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

float distance(Point A, Point B)
{
    return sqrt(pow((B.x - A.x), 2) + pow((B.y - A.y), 2));
}

void findMinRoom(Point A, Room *room, int numberOfRooms, 
    int *min_room_index, int *min_point_index, float *min_distance)
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
    }

void microTSP(Point *result, int *resIndex, int min_point_index, 
    Room room, int *visited, Point *last)
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
        (*resIndex)++;
        visited[next_corner_index] = 1;
        min_point_index = next_corner_index;
    }
    (*last).x = room.corner[min_point_index].x;
    (*last).y = room.corner[min_point_index].y;
    
}


Point *findPath(Point dockingStation, Room *room, int numberOfRooms)
{
    Point *result = malloc(sizeof(Point) * (4 * numberOfRooms + 2));
    int resIndex = 0;

    float min_distance = 99999.0;
    int min_room_index, min_point_index;
    findMinRoom(dockingStation, room, numberOfRooms, &min_room_index,
        &min_point_index, &min_distance);
    
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
        microTSP(result, &resIndex, min_point_index, room[min_room_index], visited, &last);
        findMinRoom(last, room, numberOfRooms, &min_room_index, &min_point_index, &min_distance);
    }
    result[resIndex].x = dockingStation.x;
    result[resIndex].y = dockingStation.y;

    return result;
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

int main()
{
    // 1. Definim datele de start
    int numberOfRooms = 10;
    Point dockingStation = {0, 0};
    
    // Alocam camerele (calloc pune automat visited = 0)
    Room *room = calloc(numberOfRooms, sizeof(Room));
    
    // --- MOCK DATA: 10 CAMERE ---
    
    // Cluster central (Aproape de docking station)
    room[0].corner[0] = (Point){150, 150};
    room[0].corner[1] = (Point){450, 100};
    room[0].corner[2] = (Point){500, 400};
    room[0].corner[3] = (Point){180, 450};

    room[1].corner[0] = (Point){600, 150};
    room[1].corner[1] = (Point){900, 200};
    room[1].corner[2] = (Point){850, 550};
    room[1].corner[3] = (Point){550, 450};

    room[2].corner[0] = (Point){100, -350};
    room[2].corner[1] = (Point){450, -400};
    room[2].corner[2] = (Point){500, -150};
    room[2].corner[3] = (Point){150, -100};

    // Zona de Nord / Nord-Est (Distanță medie)
    room[3].corner[0] = (Point){-200, 800};
    room[3].corner[1] = (Point){200, 800};
    room[3].corner[2] = (Point){300, 1200};
    room[3].corner[3] = (Point){-100, 1000};

    room[4].corner[0] = (Point){1500, 500};
    room[4].corner[1] = (Point){2000, 600};
    room[4].corner[2] = (Point){1900, 1000};
    room[4].corner[3] = (Point){1400, 800};

    // Extreme (Foarte îndepărtate, obligă robotul la drumuri lungi)
    room[5].corner[0] = (Point){2500, 2500};
    room[5].corner[1] = (Point){3500, 2300};
    room[5].corner[2] = (Point){3200, 3200};
    room[5].corner[3] = (Point){2400, 3000};

    room[6].corner[0] = (Point){-1500, 1500};
    room[6].corner[1] = (Point){-800, 1200};
    room[6].corner[2] = (Point){-600, 2000};
    room[6].corner[3] = (Point){-1200, 2200};

    // Zona de Sud / Sud-Vest / Sud-Est
    room[7].corner[0] = (Point){-1000, -500};
    room[7].corner[1] = (Point){-800, -500};
    room[7].corner[2] = (Point){-800, -700};
    room[7].corner[3] = (Point){-1000, -700};

    room[8].corner[0] = (Point){1500, -1000};
    room[8].corner[1] = (Point){1800, -1100};
    room[8].corner[2] = (Point){1600, -1500};
    room[8].corner[3] = (Point){1200, -1200};

    room[9].corner[0] = (Point){0, -2000};
    room[9].corner[1] = (Point){500, -2100};
    room[9].corner[2] = (Point){400, -2500};
    room[9].corner[3] = (Point){-200, -2400};

    // --- RULAREA ALGORITMULUI ---
    Point *traseu = findPath(dockingStation, room, numberOfRooms);
    
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
    free(room);
    free(traseu);
    
    return 0;
}