#include <stdbool.h>
#include "../pawns/pawns.h"
#include "../game/game.h"

extern struct GameInfo globalGameInfo;

volatile unsigned int move;
extern int blocchi_gialli[4];
extern int size;
int base0;
int base1;

//modifica bit [32:25] di move
void updateMovePlayer(int* move, char playerID) {
	
	//assegna playerID
	playerID &= 0xFF;
	*move &= 0x00FFFFFF;
	*move |= ((unsigned int)playerID << 24);
	
}

// modifica bit [24:21] di move
void updateMovePlayerWall (int* move, bool isPlayer) {
	
	// valore da spostare nel field playerMove/WallPlacement
	unsigned int valueToMove;
	
	if (isPlayer == true) {
		valueToMove = 0;
	} else {
		valueToMove = 1;
	}
	//valueToMove &= 0x00000001;
	*move &= 0xFF0FFFFF;
	
	*move |= (valueToMove << 20);

}

// modifica bit [20:17] di move
void updateMoveVerticalHorizontal(int* move, bool isVertical) {
	
	// valore da spostare nel field vertical/horizontal
	unsigned int valueToMove;
	int newMove;
	// determina se il movimento è di un player
	unsigned int isPlayerMoving;
	isPlayerMoving = (*move >> 20);
	isPlayerMoving &= 0x0000000F;
		
	if (isPlayerMoving == 0) {
		valueToMove = 0;
	} else {
		newMove = *move << 12;
		newMove = newMove >> 28;
		valueToMove= !newMove;
	}
	
	if(valueToMove == 1) { //horizontal
		int y_start = *move & 0xFF00;
		int x_start = *move & 0xFF;
		y_start = y_start >> 8;
		y_start = y_start - 4;
		*move &= 0xFFFF00FF;
		*move |= (y_start << 8);
		x_start = x_start + 4;
		*move &= 0xFFFFFF00;
		*move |= x_start;
	} else {
		int y_start = *move & 0xFF00;
		int x_start = *move & 0xFF;
		y_start = y_start >> 8;
		y_start = y_start + 4;
		*move &= 0xFFFF00FF;
		*move |= (y_start << 8);
		x_start = x_start - 4;
		*move &= 0xFFFFFF00;
		*move |= x_start;
	}
	
	*move &= 0xFFF0FFFF;
	*move |= (valueToMove << 16);
}

// modifica bit [16:8] di move
void updateMoveUp(int* move) {
	int y_start = *move & 0xFF00;
	y_start = y_start >> 8;
	//TODO: implementare limiti di posX per non uscire dalla scacchiera basandosi su coordinate massime scacchiera
	//33 offset tra le caselle
	y_start = y_start - 33;
	*move &= 0xFFFF00FF;
	*move |= (y_start << 8);
	
}

void updateMoveUpPlayer(int* move) {
	int y_start = *move & 0xFF00;
	y_start = y_start >> 8;
	//TODO: implementare limiti di posX per non uscire dalla scacchiera basandosi su coordinate massime scacchiera
	//33 offset tra le caselle
	y_start = y_start - 66;
	*move &= 0xFFFF00FF;
	*move |= (y_start << 8);
	
}

void updateMoveDown(int*	move) {
	int y_start = *move & 0xFF00;
	y_start = y_start >> 8;
	//TODO: implementare limiti di posX per non uscire dalla scacchiera basandosi su coordinate massime scacchiera
	//33 offset tra le caselle
	y_start = y_start + 33;
	*move = *move & 0xFFFF00FF;
	*move = *move | (y_start << 8);
}

void updateMoveDownPlayer(int*	move) {
	int y_start = *move & 0xFF00;
	y_start = y_start >> 8;
	//TODO: implementare limiti di posX per non uscire dalla scacchiera basandosi su coordinate massime scacchiera
	//33 offset tra le caselle
	y_start = y_start + 66;
	*move = *move & 0xFFFF00FF;
	*move = *move | (y_start << 8);
}

// modifica bit [0:7] di move
void updateMoveRight(int*	move) {
	int x_start = *move & 0xFF;

	//TODO: implementare limiti di posY per non uscire dalla scacchiera basandosi su coordinate massime scacchiera

	x_start = x_start + 33;
	*move &= 0xFFFFFF00;
	*move |= x_start;
}

void updateMoveRightPlayer(int*	move) {
	int x_start = *move & 0xFF;

	//TODO: implementare limiti di posY per non uscire dalla scacchiera basandosi su coordinate massime scacchiera

	x_start = x_start + 66;
	*move &= 0xFFFFFF00;
	*move |= x_start;
}

void updateMoveLeft(int*	move) {
	int x_start = *move & 0xFF;

	//TODO: implementare limiti di posY per non uscire dalla scacchiera basandosi su coordinate massime scacchiera

	x_start = x_start - 33;
	*move &= 0xFFFFFF00;
	*move |= x_start;
}

void updateMoveLeftPlayer(int*	move) {
	int x_start = *move & 0xFF;

	//TODO: implementare limiti di posY per non uscire dalla scacchiera basandosi su coordinate massime scacchiera

	x_start = x_start - 66;
	*move &= 0xFFFFFF00;
	*move |= x_start;
}

void initialPosition(){
	globalGameInfo.oldMove0 = 0x0000E971;
	base0 = globalGameInfo.oldMove0;
	globalGameInfo.oldMove1 = 0x01002371;
	base1 = globalGameInfo.oldMove1;
	markMoves(globalGameInfo.oldMove0, blocchi_gialli, &size, globalGameInfo.oldMove1);
}
