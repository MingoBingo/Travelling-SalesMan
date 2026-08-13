#include "export.h"
#include <stdio.h>

void exportToSVG(const char *filename, Point *traseu, int totalPuncte, 
            Room *rooms, int numberOfRooms) 
{
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Eroare la crearea fisierului SVG: %s\n", filename);
        return;
    }

    // Calculăm "rama" imaginii (Bounding Box)
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (int i = 0; i < totalPuncte; i++) {
        if (traseu[i].x < minX) minX = traseu[i].x;
        if (traseu[i].x > maxX) maxX = traseu[i].x;
        if (traseu[i].y < minY) minY = traseu[i].y;
        if (traseu[i].y > maxY) maxY = traseu[i].y;
    }
    
    int padding = 150;
    int width = maxX - minX + 2 * padding;
    int height = maxY - minY + 2 * padding;

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' viewBox='%d %d %d %d' style='background-color: #f4f4f4;'>\n",
            minX - padding, minY - padding, width, height);

    for (int i = 0; i < numberOfRooms; i++) {
        fprintf(f, "<polygon points='%d,%d %d,%d %d,%d %d,%d' fill='lightblue' stroke='blue' stroke-width='4' opacity='0.6'/>\n",
                rooms[i].corner[0].x, rooms[i].corner[0].y,
                rooms[i].corner[1].x, rooms[i].corner[1].y,
                rooms[i].corner[2].x, rooms[i].corner[2].y,
                rooms[i].corner[3].x, rooms[i].corner[3].y);
    }

    fprintf(f, "<polyline points='");
    for (int i = 0; i < totalPuncte; i++) {
        fprintf(f, "%d,%d ", traseu[i].x, traseu[i].y);
    }
    fprintf(f, "' fill='none' stroke='red' stroke-width='5' opacity='0.8' stroke-dasharray='10,10'/>\n");

    for (int i = 1; i < totalPuncte - 1; i++) {
        fprintf(f, "<circle cx='%d' cy='%d' r='6' fill='black'/>\n", traseu[i].x, traseu[i].y);
        fprintf(f, "<text x='%d' y='%d' dx='10' dy='-10' font-size='20' fill='black' font-family='Arial'>%d</text>\n", 
                traseu[i].x, traseu[i].y, i);
    }

    fprintf(f, "<circle cx='0' cy='0' r='15' fill='green'/>\n");
    fprintf(f, "<text x='0' y='0' dx='20' dy='-20' font-size='28' fill='green' font-weight='bold' font-family='Arial'>START (0,0)</text>\n");

    fprintf(f, "</svg>\n");
    fclose(f);
    
    printf("--> Succes! Harta '%s' a fost generata.\n", filename);
}

void exportChartToSVG(float *bktHistory, int historyCount, float tspCost, float opt2Cost) 
{
    FILE *f = fopen("grafic_performanta.svg", "w");
    if (!f) {
        printf("Eroare la crearea fisierului grafic SVG!\n");
        return;
    }

    int width = 1000;
    int height = 500;
    int padding = 80; 

    // Căutăm limitele pe axa Y (Cost)
    float minCost = bktHistory[0];
    float maxCost = bktHistory[0];
    for (int i = 1; i < historyCount; i++) {
        if (bktHistory[i] < minCost) minCost = bktHistory[i];
        if (bktHistory[i] > maxCost) maxCost = bktHistory[i];
    }
    
    // Includem și liniile euristice în cadru
    if (tspCost < minCost) minCost = tspCost;
    if (tspCost > maxCost) maxCost = tspCost;
    if (opt2Cost < minCost) minCost = opt2Cost;
    if (opt2Cost > maxCost) maxCost = opt2Cost;

    float range = maxCost - minCost;
    if (range == 0) range = 1.0;
    maxCost += range * 0.20; // Extra spatiu sus
    minCost -= range * 0.10;
    range = maxCost - minCost;

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 %d %d' style='background-color: #ffffff; font-family: Arial;'>\n", width, height);

    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black' stroke-width='2'/>\n", padding, padding, padding, height - padding);
    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black' stroke-width='2'/>\n", padding, height - padding, width - padding, height - padding);

    // 1. Linia Albastră (BKT Evolution)
    float dx = (float)(width - 2 * padding) / (historyCount > 1 ? historyCount - 1 : 1);
    fprintf(f, "<polyline points='");
    for (int i = 0; i < historyCount; i++) {
        float x = padding + i * dx;
        float y = height - padding - ((bktHistory[i] - minCost) / range) * (height - 2 * padding);
        fprintf(f, "%.2f,%.2f ", x, y);
    }
    fprintf(f, "' fill='none' stroke='blue' stroke-width='4' opacity='0.7'/>\n");

    for (int i = 0; i < historyCount; i++) {
        float x = padding + i * dx;
        float y = height - padding - ((bktHistory[i] - minCost) / range) * (height - 2 * padding);
        fprintf(f, "<circle cx='%.2f' cy='%.2f' r='5' fill='blue'/>\n", x, y);
        if (i == 0 || i == historyCount - 1 || i % 4 == 0) {
            fprintf(f, "<text x='%.2f' y='%.2f' dx='-15' dy='-12' font-size='12' fill='darkblue'>%d</text>\n", x, y, (int)bktHistory[i]);
        }
    }

    // 2. Linia Roșie (Standard TSP)
    float tspY = height - padding - ((tspCost - minCost) / range) * (height - 2 * padding);
    fprintf(f, "<line x1='%d' y1='%.2f' x2='%d' y2='%.2f' stroke='red' stroke-width='3' stroke-dasharray='10,10'/>\n", 
            padding, tspY, width - padding, tspY);
    fprintf(f, "<text x='%d' y='%.2f' dx='10' dy='-10' font-size='16' fill='red' font-weight='bold'>Cost TSP: %d</text>\n", 
            width - 250, tspY, (int)tspCost);

    // 3. Linia Verde (Random 2-OPT)
    float opt2Y = height - padding - ((opt2Cost - minCost) / range) * (height - 2 * padding);
    fprintf(f, "<line x1='%d' y1='%.2f' x2='%d' y2='%.2f' stroke='green' stroke-width='3' stroke-dasharray='15,5'/>\n", 
            padding, opt2Y, width - padding, opt2Y);
    // Dacă TSP și 2-OPT sunt foarte apropiate ca scor, mutăm textul puțin ca să nu se suprapună urât
    int textOffset = (abs((int)(tspY - opt2Y)) < 20) ? 20 : -10; 
    fprintf(f, "<text x='%d' y='%.2f' dx='10' dy='-10' font-size='16' fill='green' font-weight='bold'>Cost 2-OPT: %d</text>\n", 
            width - 450, opt2Y, (int)opt2Cost);

    fprintf(f, "<text x='%d' y='%d' font-size='22' font-weight='bold' fill='black'>Evolutie BKT vs TSP vs 2-OPT</text>\n", width / 2 - 200, padding - 30);

    fprintf(f, "</svg>\n");
    fclose(f);

    printf("--> Succes! Graficul 'grafic_performanta.svg' a fost actualizat.\n");
}