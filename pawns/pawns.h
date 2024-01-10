#include <stdbool.h>
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#define UPPERSPACE 29


void movePawns(int, bool);
void markMoves(int, int*, int*, int);
void removeMarkedMoves(int*, int*);
void drawPawns(int position);
bool checkWall(int, int);
bool checkPlayerNearOver(int, int, int);
bool checkPlayerNearOverTmp(int, int);
bool checkPlayerNearUnder(int, int, int);
bool checkPlayerNearUnderTmp(int, int);
bool checkPlayerNearRight(int, int, int);
bool checkRightTmp(int, int);
bool checkPlayerNearLeft(int, int, int);
bool checkLeftTmp(int, int);
bool checkWallsOver(int, int);
bool checkWallsUnder(int, int);
bool checkWallsUnderTmp(int, int);
bool checkWallsOverTmp(int, int);
bool pawsArrived(int);
void endGame(int);
void pawnMoved(void);

