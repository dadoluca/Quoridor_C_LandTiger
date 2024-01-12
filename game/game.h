#include <stdbool.h>

#ifndef GAME_H
#define GAME_H

#define MAX_WALLS_PER_USER 8
#define PLAYER_0 0	//Green
#define	PLAYER_1 1	//Blue
#define WALL_MODE 0
#define	TOKEN_MODE 1
#define	DIRECTION_RIGHT 0
#define	DIRECTION_DOWN 1
#define	DIRECTION_LEFT 2
#define	DIRECTION_UP 3


struct GameInfo {
    int last_move_p0;															//last 32 bit move string of player 0
		int last_move_p1;															//		"					"				"						1
		int current_turn_player; 											//PLAYER_0 or PLAYER_1
		int current_move_mode;												//0:WALL_MODE  1:TOKEN_MODE
};
extern struct GameInfo globalGameInfo;

struct Walls {
    unsigned int index;														//current wallsList wall index
    unsigned int wallsList[MAX_WALLS_PER_USER*2]; //sequence of placed wall moves
};
extern struct Walls globalWalls;

static void drawInitBoard();
static void drawInitTokens();
void initGame();

#endif
