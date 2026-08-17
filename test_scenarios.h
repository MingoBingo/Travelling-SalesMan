// test_scenarios.h
//
// Cazuri extreme pentru testarea findPath / findRandom2OPTPath / findPathBkt.
// Fiecare functie umple un array Room* deja alocat (calloc(numberOfRooms, sizeof(Room)))
// cu un scenariu specific. In main.c, inlocuieste blocul "MOCK DATA" si linia
// "int numberOfRooms = 20;" cu una din perechile (#define N_..., loadScenario_...).
//
// Exemplu de folosire in main():
//
//     int numberOfRooms = N_CIRCLE;
//     Room *room = calloc(numberOfRooms, sizeof(Room));
//     loadScenario_Circle(room, numberOfRooms);
//
#ifndef TEST_SCENARIOS_H
#define TEST_SCENARIOS_H

#define M_PI 3.14159265358979323846

#include <math.h>
#include "geometry.h"

// ============================================================
// 1. UN SINGUR ROOM
// Testeaza cazurile de baza cu n=1: bucle care presupun n>=2 (in special
// for(i=0;i<numberOfRooms-1;i++) din 2-OPT nu se executa deloc -> foundImprovement
// ramane mereu 0 -> while(foundImprovement) nu porneste niciodata. Verifica ca
// totalCost/path-ul se calculeaza totusi corect din prima trecere prin DP.
// ============================================================
#define N_SINGLE 1
static inline void loadScenario_SingleRoom(Room *room)
{
    room[0].corner[0] = (Point){100, 100};
    room[0].corner[1] = (Point){300, 120};
    room[0].corner[2] = (Point){280, 320};
    room[0].corner[3] = (Point){90, 300};
}

// ============================================================
// 2. DOUA ROOM-URI
// Cel mai mic caz in care bucla de 2-OPT chiar ruleaza (o singura pereche i=0,j=1).
// Verifica manual ca rezultatul e identic cu varianta ne-inversata daca aceea
// e deja optima (adica reverseOrderRooms nu trebuie sa strice nimic).
// ============================================================
#define N_TWO 2
static inline void loadScenario_TwoRooms(Room *room)
{
    room[0].corner[0] = (Point){500, 500};   room[0].corner[1] = (Point){700, 520};
    room[0].corner[2] = (Point){680, 720};   room[0].corner[3] = (Point){480, 700};

    room[1].corner[0] = (Point){-500, -500}; room[1].corner[1] = (Point){-300, -480};
    room[1].corner[2] = (Point){-320, -280}; room[1].corner[3] = (Point){-520, -300};
}

// ============================================================
// 3. ROOM DEGENERAT (arie zero)
// Toate cele 4 colturi identice -> toate cele 12 permutari inOutPermutations
// dau distance()==0. Testeaza impartiri/comparatii care presupun cost>0 si
// tie-breaking cand minCost e 0 pentru mai multe optiuni deodata.
// ============================================================
#define N_DEGENERATE 3
static inline void loadScenario_ZeroAreaRoom(Room *room)
{
    room[0].corner[0] = (Point){0, 0};
    room[0].corner[1] = (Point){0, 0};
    room[0].corner[2] = (Point){0, 0};
    room[0].corner[3] = (Point){0, 0};

    room[1].corner[0] = (Point){400, 400}; room[1].corner[1] = (Point){600, 420};
    room[1].corner[2] = (Point){580, 620}; room[1].corner[3] = (Point){380, 600};

    // colturi coliniare (patrulater "turtit" intr-o linie, arie ~0 dar nu chiar 0)
    room[2].corner[0] = (Point){-200, -200}; room[2].corner[1] = (Point){-100, -200};
    room[2].corner[2] = (Point){0, -200};    room[2].corner[3] = (Point){100, -200};
}

// ============================================================
// 4. ROOM AUTO-INTERSECTAT (bowtie / non-convex)
// Colturile NU sunt in ordine convexa -> edge-urile poligonului se
// intersecteaza. Codul de geometrie/SVG presupune probabil un patrulater
// simplu (convex, colturi in ordine); vezi daca exportToSVG deseneaza
// ceva sanatos sau un X, si daca inOutPermutations produce distante ciudate.
// ============================================================
#define N_SELFINTERSECT 1
static inline void loadScenario_BowtieRoom(Room *room)
{
    room[0].corner[0] = (Point){0, 0};
    room[0].corner[1] = (Point){200, 200};   // colturi 1 si 3 inversate fata
    room[0].corner[2] = (Point){200, 0};     // de ordinea "normala" in sens orar
    room[0].corner[3] = (Point){0, 200};
}

// ============================================================
// 5. ROOM CHIAR PE DOCKING STATION
// Un colt al camerei coincide cu (0,0) -> distance(dockingStation, corner) == 0
// pentru unele permutari. Testeaza ties in alegerea firstIndex.
// ============================================================
#define N_AT_DOCK 2
static inline void loadScenario_RoomAtDocking(Room *room)
{
    room[0].corner[0] = (Point){0, 0};
    room[0].corner[1] = (Point){200, 20};
    room[0].corner[2] = (Point){180, 220};
    room[0].corner[3] = (Point){-20, 200};

    room[1].corner[0] = (Point){1000, 1000}; room[1].corner[1] = (Point){1200, 1020};
    room[1].corner[2] = (Point){1180, 1220}; room[1].corner[3] = (Point){980, 1200};
}

// ============================================================
// 6. DOUA ROOM-URI IDENTICE (suprapuse perfect)
// Testeaza egalitate stricta de cost intre doua "camere" distincte ca index
// dar identice ca geometrie -> multiple solutii optime simultan.
// ============================================================
#define N_OVERLAP 2
static inline void loadScenario_OverlappingRooms(Room *room)
{
    Point c0 = {300, 300}, c1 = {500, 320}, c2 = {480, 520}, c3 = {280, 500};
    room[0].corner[0] = c0; room[0].corner[1] = c1; room[0].corner[2] = c2; room[0].corner[3] = c3;
    room[1].corner[0] = c0; room[1].corner[1] = c1; room[1].corner[2] = c2; room[1].corner[3] = c3;
}

// ============================================================
// 7. COORDONATE URIASE
// distance() foloseste sqrt pe diferente mari -> verifica overflow/precizie
// in float, si mai ales calculul lui width/height (int) din exportToSVG,
// care poate depasi INT_MAX daca minX/maxX sunt f. departate.
// ============================================================
#define N_HUGE 3
static inline void loadScenario_HugeCoordinates(Room *room)
{
    room[0].corner[0] = (Point){2000000, 2000000};   room[0].corner[1] = (Point){2000300, 2000030};
    room[0].corner[2] = (Point){2000280, 2000330};   room[0].corner[3] = (Point){2000010, 2000300};

    room[1].corner[0] = (Point){-2000000, -2000000}; room[1].corner[1] = (Point){-1999700, -1999970};
    room[1].corner[2] = (Point){-1999720, -1999670}; room[1].corner[3] = (Point){-1999990, -1999700};

    room[2].corner[0] = (Point){2000000, -2000000};  room[2].corner[1] = (Point){2000300, -1999970};
    room[2].corner[2] = (Point){2000280, -1999670};  room[2].corner[3] = (Point){2000010, -1999700};
}

// ============================================================
// 8. CLUSTER FOARTE STRANS (precizie float)
// Toate camerele intr-o zona de cateva zeci de unitati -> distantele intre
// permutari devin foarte apropiate, bun test pentru stabilitatea comparatiilor
// "<" in prezenta zgomotului de rotunjire float.
// ============================================================
#define N_CLUSTER 6
static inline void loadScenario_TightCluster(Room *room, int n)
{
    for (int i = 0; i < n; ++i) {
        int bx = (i % 3) * 15;
        int by = (i / 3) * 15;
        room[i].corner[0] = (Point){bx,     by};
        room[i].corner[1] = (Point){bx + 10, by + 1};
        room[i].corner[2] = (Point){bx + 9,  by + 11};
        room[i].corner[3] = (Point){bx - 1,  by + 10};
    }
}

// ============================================================
// 9. TOATE CAMERELE COLINIARE (pe o linie dreapta)
// Solutia optima e trivial "de la stanga la dreapta" (sau invers). Bun test
// ca 2-OPT converge la ordinea corecta indiferent de shuffle-ul initial, si
// ca backtracking-ul (daca il pornesti pe N mic) gaseste acelasi optim.
// ============================================================
#define N_COLLINEAR_MAX 30
static inline void loadScenario_Collinear(Room *room, int n)
{
    for (int i = 0; i < n; ++i) {
        int x = i * 300 - (n * 150);
        room[i].corner[0] = (Point){x,       0};
        room[i].corner[1] = (Point){x + 120, 10};
        room[i].corner[2] = (Point){x + 110, 130};
        room[i].corner[3] = (Point){x - 10,  120};
    }
}

// ============================================================
// 10. CAMERE PE UN CERC
// Optimul geometric evident e "in jurul cercului". Caz clasic in care
// 2-OPT pe baza de swap-uri random poate ramane blocat intr-un optim local
// (o coarda peste cerc) daca nu exploreaza suficiente vecinatati.
// ============================================================
#define N_CIRCLE_MAX 40
static inline void loadScenario_Circle(Room *room, int n)
{
    double radius = 1500.0;
    for (int i = 0; i < n; ++i) {
        double angle = (2.0 * M_PI * i) / n;
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));
        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 100, cy + 10};
        room[i].corner[2] = (Point){cx + 90,  cy + 110};
        room[i].corner[3] = (Point){cx - 10,  cy + 100};
    }
}

// ============================================================
// 11. GRID REGULAT (n trebuie sa fie patrat perfect: 4, 9, 16, 25, 36...)
// Caz clasic "greu" pentru euristici gen 2-OPT (multe optime locale la
// distante similare intre randuri/coloane adiacente).
// ============================================================
#define N_GRID_MAX 36
static inline void loadScenario_Grid(Room *room, int n)
{
    int side = (int)sqrt((double)n);
    int step = 400;
    for (int i = 0; i < n; ++i) {
        int gx = (i % side) * step;
        int gy = (i / side) * step;
        room[i].corner[0] = (Point){gx,       gy};
        room[i].corner[1] = (Point){gx + 150, gy + 15};
        room[i].corner[2] = (Point){gx + 135, gy + 165};
        room[i].corner[3] = (Point){gx - 15,  gy + 150};
    }
}

// ============================================================
// 12. N MARE PENTRU STRES DE PERFORMANTA (doar TSP / 2-OPT, NU backtracking)
// Backtracking-ul e O(n! * 12^n)-ish, deci NU il porni cu asta peste ~10-12
// camere (RUN_BACKTRACKING = 0). Foloseste asta ca sa vezi cat dureaza
// findRandom2OPTPath / findPath cand n creste mult (100, 300, 1000...).
// ============================================================
static inline void loadScenario_RandomStress(Room *room, int n, int spread)
{
    for (int i = 0; i < n; ++i) {
        int bx = (rand() % (2 * spread)) - spread;
        int by = (rand() % (2 * spread)) - spread;
        room[i].corner[0] = (Point){bx,       by};
        room[i].corner[1] = (Point){bx + 120, by + 12};
        room[i].corner[2] = (Point){bx + 108, by + 132};
        room[i].corner[3] = (Point){bx - 12,  by + 120};
    }
}

// ============================================================
// 13. CAMERE CONCENTRICE (Inel în inel)
// Testează scenariul în care ai un set de camere pe un cerc mic 
// și alt set pe un cerc mare. 2-opt-ul tău riscă să se blocheze 
// făcând intersecții între cercuri (treceri bruște de la interior la exterior).
// ============================================================
#define N_CONCENTRIC_MAX 30
static inline void loadScenario_Concentric(Room *room, int n)
{
    double r_inner = 500.0;
    double r_outer = 1500.0;
    int half = n / 2;

    for (int i = 0; i < n; ++i) {
        double radius = (i < half) ? r_inner : r_outer;
        double angle = (2.0 * M_PI * (i % half)) / half;
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 80,  cy + 10};
        room[i].corner[2] = (Point){cx + 70,  cy + 90};
        room[i].corner[3] = (Point){cx - 10,  cy + 80};
    }
}

// ============================================================
// 14. CORIDOR LUNG ȘI ÎNGUST (TUNEL)
// Camerele sunt înșiruite în zigzag printr-un coridor foarte îngust.
// Forțează ordinea de traversare să nu se întoarcă haotic, 
// testând cum se descurcă DP-ul când distanțele de întoarcere sunt uriașe
// comparativ cu pasul mic dintre camere.
// ============================================================
#define N_TUNNEL_MAX 30
static inline void loadScenario_Tunnel(Room *room, int n)
{
    for (int i = 0; i < n; ++i) {
        int x = (i % 2 == 0) ? 0 : 200;
        int y = (i / 2) * 150;

        room[i].corner[0] = (Point){x,      y};
        room[i].corner[1] = (Point){x + 80, y + 5};
        room[i].corner[2] = (Point){x + 75, y + 100};
        room[i].corner[3] = (Point){x - 5,  y + 95};
    }
}

// ============================================================
// 15. DOCKING STATION ÎN CENTRUL UNUI CLUSTER RADIAL
// Docking-ul este exact în (0,0), iar camerele sunt împrăștiate 
// radial în toate direcțiile la distanțe egale. 
// Testează eficiența alegerii primei camere (`firstIndex`).
// ============================================================
#define N_STAR_MAX 32
static inline void loadScenario_StarCluster(Room *room, int n)
{
    double radius = 1000.0;
    for (int i = 0; i < n; ++i) {
        double angle = (2.0 * M_PI * i) / n;
        // Camerele sunt puțin "rotite" ca o elice față de centru
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 50,  cy + 10};
        room[i].corner[2] = (Point){cx + 40,  cy + 60};
        room[i].corner[3] = (Point){cx - 10,  cy + 50};
    }
}

// ============================================================
// 16. CAMERE DISPROPORȚIONATE (O cameră gigant și restul mici)
// O cameră are coordonatele la distanțe de 10.000 de unități, 
// iar restul sunt pitice în (0,0). Testează stabilitatea preciziei 
// float în DP la diferențe de scară enorme.
// ============================================================
#define N_GIANT_DRAIN 5
static inline void loadScenario_GiantAndDwarfs(Room *room)
{
    // Camera gigant
    room[0].corner[0] = (Point){-5000, -5000};
    room[0].corner[1] = (Point){ 5000, -5000};
    room[0].corner[2] = (Point){ 5000,  5000};
    room[0].corner[3] = (Point){-5000,  5000};

    // Camerele pitice în jur de (0,0)
    for (int i = 1; i < 5; ++i) {
        int bx = (i * 40) - 100;
        int by = 0;
        room[i].corner[0] = (Point){bx,      by};
        room[i].corner[1] = (Point){bx + 20, by + 2};
        room[i].corner[2] = (Point){bx + 18, by + 22};
        room[i].corner[3] = (Point){bx - 2,  by + 20};
    }
}

#define N_CIRCLE 10
static inline void loadScenario_Circle10(Room *room)
{
    double radius = 1500.0;
    int n = 10;
    for (int i = 0; i < n; ++i) {
        double angle = (2.0 * M_PI * i) / n;
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));
        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 100, cy + 10};
        room[i].corner[2] = (Point){cx + 90,  cy + 110};
        room[i].corner[3] = (Point){cx - 10,  cy + 100};
    }
}

// ============================================================
// 13. TREI CAMERE ÎN TRIUNGHI ECHILATERAL
// Foarte bun pentru backtracking pe N mic (N=3). Testează simetria 
// și alegerea optimă a sensului de parcurgere (orar vs antiorar).
// ============================================================
#define N_TRIANGLE 3
static inline void loadScenario_Triangle(Room *room)
{
    double radius = 1000.0;
    for (int i = 0; i < 3; ++i) {
        double angle = (2.0 * M_PI * i) / 3.0;
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 80,  cy + 10};
        room[i].corner[2] = (Point){cx + 70,  cy + 90};
        room[i].corner[3] = (Point){cx - 10,  cy + 80};
    }
}

// ============================================================
// 14. PATRU CAMERE ÎN PATRATER (GRID 2x2)
// N=4 este un caz excelent pentru backtracking complet (4! * 12^4 
// este manevrabil pe calculator modern). Testează intersecțiile de diagonale.
// ============================================================
#define N_SQUARE_GRID 4
static inline void loadScenario_SquareGrid(Room *room)
{
    int coords[4][2] = {
        {-500,  500}, { 500,  500},
        {-500, -500}, { 500, -500}
    };

    for (int i = 0; i < 4; ++i) {
        int cx = coords[i][0];
        int cy = coords[i][1];

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 100, cy + 10};
        room[i].corner[2] = (Point){cx + 90,  cy + 110};
        room[i].corner[3] = (Point){cx - 10,  cy + 100};
    }
}

// ============================================================
// 15. CINCI CAMERE ÎN FORMĂ DE CRUCE / PLUS (N=5)
// Un centru și 4 brațe simetrice. Testează cum alege backtracking-ul 
// camera din centru prima sau la mijlocul traseului.
// ============================================================
#define N_CROSS 5
static inline void loadScenario_Cross(Room *room)
{
    // Centrul
    room[0].corner[0] = (Point){0, 0};
    room[0].corner[1] = (Point){80, 10};
    room[0].corner[2] = (Point){70, 90};
    room[0].corner[3] = (Point){-10, 80};

    // Sus, Jos, Stânga, Dreapta
    int offsets[4][2] = {{0, 800}, {0, -800}, {-800, 0}, {800, 0}};
    for (int i = 1; i < 5; ++i) {
        int cx = offsets[i-1][0];
        int cy = offsets[i-1][1];

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 80,  cy + 10};
        room[i].corner[2] = (Point){cx + 70,  cy + 90};
        room[i].corner[3] = (Point){cx - 10,  cy + 80};
    }
}

// ============================================================
// 16. ȘASE CAMERE ÎN LINIE DREAPTĂ (N=6)
// Versiune mică a scenariului coliniar, perfectă pentru a rula 
// backtracking-ul rapid și a compara exact costul cu DP-ul sau 2-opt.
// ============================================================
#define N_LINE_SIX 6
static inline void loadScenario_LineSix(Room *room)
{
    for (int i = 0; i < 6; ++i) {
        int x = i * 250 - 625; // Centrat în jur de 0
        int y = 0;

        room[i].corner[0] = (Point){x,      y};
        room[i].corner[1] = (Point){x + 80, y + 10};
        room[i].corner[2] = (Point){x + 70, y + 90};
        room[i].corner[3] = (Point){x - 10, y + 80};
    }
}


// ============================================================
// 17. OPT CAMERE ÎN CERC (N=8)
// Versiunea mică a cercului, perfectă pentru backtracking. 
// Testează dacă algoritmul găsește drumul optim pe inel fără a tăia prin interior.
// ============================================================
#define N_CIRCLE_EIGHT 8
static inline void loadScenario_CircleEight(Room *room)
{
    double radius = 1200.0;
    int n = 8;
    for (int i = 0; i < n; ++i) {
        double angle = (2.0 * M_PI * i) / n;
        int cx = (int)(radius * cos(angle));
        int cy = (int)(radius * sin(angle));

        room[i].corner[0] = (Point){cx,       cy};
        room[i].corner[1] = (Point){cx + 80,  cy + 10};
        room[i].corner[2] = (Point){cx + 70,  cy + 90};
        room[i].corner[3] = (Point){cx - 10,  cy + 80};
    }
}

// ============================================================
// 18. DOUĂ GRUPURI A CÂTE 4 CAMERE (CLUSTERE SEPARATE) (N=8)
// Două insule de camere depărtate una de alta (un cluster în stânga, 
// altul în dreapta). Testează ordinea în care backtracking-ul alege 
// să treacă de la un cluster la celălalt.
// ============================================================
#define N_TWO_CLUSTERS_EIGHT 8
static inline void loadScenario_TwoClustersEight(Room *room)
{
    // Cluster stânga (camerele 0-3)
    for (int i = 0; i < 4; ++i) {
        int bx = -1500 + (i % 2) * 200;
        int by = -500 + (i / 2) * 200;
        room[i].corner[0] = (Point){bx,      by};
        room[i].corner[1] = (Point){bx + 80, by + 10};
        room[i].corner[2] = (Point){bx + 70, by + 90};
        room[i].corner[3] = (Point){bx - 10, by + 80};
    }
    // Cluster dreapta (camerele 4-7)
    for (int i = 4; i < 8; ++i) {
        int bx = 1500 + ((i - 4) % 2) * 200;
        int by = -500 + ((i - 4) / 2) * 200;
        room[i].corner[0] = (Point){bx,      by};
        room[i].corner[1] = (Point){bx + 80, by + 10};
        room[i].corner[2] = (Point){bx + 70, by + 90};
        room[i].corner[3] = (Point){bx - 10, by + 80};
    }
}

// ============================================================
// 19. GRID 2x4 (N=8)
// Un dreptunghi format din 2 rânduri și 4 coloane. 
// Forțează căutarea pe o matrice regulată de noduri.
// ============================================================
#define N_GRID_2X4 8
static inline void loadScenario_Grid2x4(Room *room)
{
    int step_x = 400;
    int step_y = 400;
    for (int i = 0; i < 8; ++i) {
        int gx = (i % 4) * step_x;
        int gy = (i / 4) * step_y;

        room[i].corner[0] = (Point){gx,       gy};
        room[i].corner[1] = (Point){gx + 90,  gy + 10};
        room[i].corner[2] = (Point){gx + 80,  gy + 90};
        room[i].corner[3] = (Point){gx - 10,  gy + 80};
    }
}
#endif // TEST_SCENARIOS_H