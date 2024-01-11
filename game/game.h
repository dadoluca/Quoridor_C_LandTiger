#include <stdbool.h>

#ifndef GAME_H
#define GAME_H

#define MAX_WALLS_PER_USER 8
#define WALL_MODE 0
#define	TOKEN_MODE 1

struct GameInfo {
    int last_move_p0;															//last 32 bit move string of player 0
		int last_move_p1;															//		"					"				"						1
		int current_turn_player; 											//0 or 1
		int current_move_mode;																	//0:wall move  1:token move

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
