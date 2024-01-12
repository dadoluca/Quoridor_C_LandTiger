#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../pawns/pawns.h"
#include "../game/game.h"

extern struct Walls globalWalls;
extern int current_default_beginning_wall;

int getIndexPawn(int posOldMove);


// TODO: verificare correttezza terza colonna
const int matPos[] = {
	0xE90E,0xE92F,0x5050,0xE971,0xE992,0xE9B3,0xE9D4,
	0xC80E,0xC82F,0xC850,0xC871,0xC892,0xC8B3,0xC8D4,
	0xA70E,0xA72F,0xA750,0xA771,0xA792,0xA7B3,0xA7D4,
	0x860E,0x862F,0x8650,0x8671,0x8692,0x86B3,0x86D4,
	0x650E,0x652F,0x6550,0x6571,0x6592,0x65B3,0x65D4,
	0x440E,0x442F,0x4450,0x4471,0x4492,0x44B3,0x44D4,
	0x230E,0x232F,0x2350,0x2371,0x2392,0x23B3,0x23D4,
}; 

const int matWallVertical[] = {
	0xC20A, 0xC243, 0xC264, 0xC285, 0xC2A6, 0xC2C7,
	0xA10A, 0xA143, 0xA164, 0xA185, 0xA1A6, 0xA1C7,
	0x800A, 0x8043, 0x8064, 0x8085, 0x80A6, 0x80C7,
	0x5F0A, 0x5F43, 0x5F64, 0x5F85, 0x5FA6, 0x5FC7, 
	0x3E0A, 0x3E43, 0x3E64, 0x3E85, 0x3EA6, 0x3EC7,
	0x1D0A, 0x1D43, 0x1D64, 0x1D85, 0x1DA6, 0x1DC7
};
	
const int matWallHorizontal[] = {
	0xDF05, 0xDF26, 0xDF47, 0xDF68, 0xDF89, 0xDFAA,
	0xBE05, 0xBE26, 0xBE47, 0xBE68, 0xBE89, 0xBEAA,
	0x9D05, 0x9D26, 0x9D47, 0x9D68, 0x9D89, 0x9DAA,
	0x7C05, 0x7C26, 0x7C47, 0x7C68, 0x7C89, 0x7CAA,
	0x5B05, 0x5B26, 0x5B47, 0x5B68, 0x5B89, 0x5BAA,
	0x3A05, 0x3A26, 0x3A47, 0x3A68, 0x3A89, 0x3AAA,
};

int board[49];

//inizializza board, richiamata ogni volta che si vuole posizionare un muro
void initBoard(int oldMove){
	int i;
	
	int posOldMove = oldMove & 0xFFFF;
	
	for(i=0;i<49;i++){
		board[i] = 0;
	}
	
	// inizializza a 1 casella dove si trova il giocatore
	board[getIndexPawn(posOldMove)]=1;
}

int getIndexPawn(int posOldMove){
	int i;
	int res;
	posOldMove = posOldMove & 0xFFFF;
	for(i=0;i<49;i++){
		if(posOldMove==matPos[i]){
			res=i; 
		} 
	}
	return res;
}


void stepIterativo(int posOldMove, int* matMove){
	int i_pawn = getIndexPawn(posOldMove);
	
	if((!checkWall(posOldMove,DIRECTION_RIGHT)) && (i_pawn+1)%7!=0 && matMove[i_pawn+1] != 2){//se non c'è muro right e non è l'ultima colonna
		matMove[i_pawn+1]=1;//cella raggiunta
	}
	if((!checkWall(posOldMove,DIRECTION_DOWN)) && i_pawn>6 && matMove[i_pawn-7] != 2){//se non c'è muro under e i_pawn non è la prima riga in basso
		matMove[i_pawn-7]=1;
	}
	if((!checkWall(posOldMove,DIRECTION_LEFT)) && i_pawn%7!=0 && matMove[i_pawn-1] != 2){//se non c'è muro left e non è prima colonna
		matMove[i_pawn-1]=1;
	}
	if((!checkWall(posOldMove,DIRECTION_UP)) && i_pawn<42 && matMove[i_pawn+7] != 2){//se non c'è muro up
		matMove[i_pawn+7]=1;
	}
	
	matMove[i_pawn] = 2; //cella raggiunta e ho esplorato i suoi dintorni
}


// chiamare a fine procedura check, libera memoria allocata per vettore usato dall'algoritmo e mette a null il puntatore
// evita memory leak
void freeBoard(int* matMove) {
    free(matMove);
		matMove = NULL;
}

// da chiamare una volta per muro iterando sulla lista muri
// input: elemento globalWalls -> output: indice della posizione nella tabella Horizontal o Vertical da 0 a 35
int getPosMuro(int wallMove){
		int i;

		int wallIndex = 0xFFFFFFFF;	// imposto valore di default per debug
		
	
		// calcolo direzione wallMove
		char direction = wallMove & 0xF0000;
		direction = direction >> 16;
		direction = direction & 0xF;
	
		// check se wallMOve è verticale
		if (direction == 0) {
			for(i=0;i<36;i++){
				int storedPosition = matWallVertical[i];
				int wallMovePosition = wallMove & 0xFFFF;
				if (storedPosition == wallMovePosition){
					wallIndex = i;
				}
			}
		}
		// wallMove è orizzontale
		else {
			for(i=0;i<36;i++){
				int storedPosition = matWallHorizontal[i];
				int wallMovePosition = wallMove & 0xFFFF;
				if (storedPosition == wallMovePosition){
					wallIndex = i;
				}
			}
		}
	return wallIndex;
		
}

bool foundWinRow(int* board, int currPlayer){
	int i;

	if(currPlayer == 1){	//Devo controllare l'opposto perchè quando piazzo muro blu devo controllare se giocatore rosso può arrivare alla fine
		for(i=0; i<7; i++){
			if(board[i] == 1) { return true; }
		}
	} else {
		for(i=42; i<49; i++){
			if(board[i] == 1) { return true; }
		}
	}
	return false;
}

bool noOneInBoard(int* board){
	int i;

	for(i=0; i<49; i++){
		if(board[i] == 1) { return false; }
	}
	return true;
}

void insertWallForCheck(int wall){
	if(wall != 0){
			globalWalls.wallsList[globalWalls.index] = wall;
			globalWalls.index = globalWalls.index + 1;
		} else {
			wall = wall == 0 ? current_default_beginning_wall : wall;
			globalWalls.wallsList[globalWalls.index] = wall;
			globalWalls.index = globalWalls.index + 1;
		}
}

void deleteWallAfterCheck(int wall){
	globalWalls.index = globalWalls.index - 1;
}

bool checkPermanentPositionWall(int position, int wall) {
	bool continueFor = true;
	bool canPlace = false;
	int index;
	int currPlayer = position & 0xF000000;
	currPlayer = currPlayer >> 24;
	initBoard(position);

	// i da 0 a 48 (1 nelle due ultime righe, oppure nessun 1 oltre il 2	, if board[i] == 1 richiami stepIterativo
	insertWallForCheck(wall);
	while(continueFor){
		for(index = 0; index<49; index++){
			if(board[index] == 1) {
				stepIterativo(matPos[index], board);
				if(foundWinRow(board, currPlayer)){
					deleteWallAfterCheck(wall);
					continueFor = false;
					canPlace = true;
				}
			}
		}
		if(noOneInBoard(board)) {
			deleteWallAfterCheck(wall);
			continueFor = false;
			canPlace = false;
		}
	}
	return canPlace;
}





