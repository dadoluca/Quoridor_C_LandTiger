#include <stdbool.h>
#define MAX_WALLS_PER_USER 8
#ifndef WALLS_H
#define WALLS_H

struct walls {
    unsigned int index;
    unsigned int wallsList[16];
};

// Dichiarazione della variabile globale
extern struct walls globalWalls;

#endif

void draw_wall_wrapper(int move, bool cancel);
void draw_wall(int posX_start, int posY_start, int posX_end, int posY_end, int vertical, int player, bool cancel);
int totalPlayerWalls(int);
bool finishedWalls(int);
void writeWarningMessage(int, bool);
void updateCountWalls(int);
bool checkPosition(int,int);
int calculate_centre(int, int, int);