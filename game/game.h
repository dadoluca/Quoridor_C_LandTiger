#include <stdbool.h>

#ifndef GAME_H
#define GAME_H
extern int oldMove0;
extern int oldMove1;
struct GameInfo {
    int oldMove0;
		int oldMove1;
		int currentPlayer;
};

extern struct GameInfo globalGameInfo;

#endif
