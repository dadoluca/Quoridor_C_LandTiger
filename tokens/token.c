#include "token.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../walls/walls.h"
#include <stdbool.h>
#include "../timer/timer.h"
#include "../joystick/joystick.h"
#include "../game/game.h"
bool checkWallsRightTmp(int move, int enemy);
bool checkWallsLeftTmp(int move, int enemy);

extern struct GameInfo globalGameInfo;

int x;
 int y;
volatile int user;
bool vertical = false;
int k;
int xCoord;
int yCoord;
extern int m;


void movePawns(int newMove, bool cancel){
	int i;	
	
	user = newMove >> 24;
	x = newMove & 0xFF;
	y = newMove & 0xFF00;
	y = y >> 8;

	if(cancel){
		for(i=0;i<17;i++){	
			LCD_DrawLine(x, y , x+10, y, White);
			y++;
		}
	} else {
		if(user == PLAYER_1){
			for(i=0;i<17;i++){	
				LCD_DrawLine(x, y , x+10, y  , Blue);
				LCD_DrawLine(x, y, x+10, y , Blue);
				if(i>=4 && i<=6){
					LCD_DrawLine(x+2, y, x+4, y , White);
					LCD_DrawLine(x+6, y, x+8, y , White);
				}else if(i>=11 && i<=13){
					LCD_DrawLine(x+2, y, x+8, y , Red);
				}
				y++;
			}
		} else if (user == PLAYER_0){
			for(i=0;i<17;i++){	
				LCD_DrawLine(x, y , x+10, y  , Green);
				LCD_DrawLine(x, y, x+10, y , Green);
				if(i>=4 && i<=6){
					LCD_DrawLine(x+2, y, x+4, y , Black);
					LCD_DrawLine(x+6, y, x+8, y , Black);
				}else if(i>=11 && i<=13){
					LCD_DrawLine(x+2, y, x+8, y , Red);
				}
				y++;
			}
		}
	}
}

bool checkPlayerNearOver(int xPawns, int yPawns, int positionEnemy) { // quando il nemico è sopra di me
	x = positionEnemy & 0xFF;
	y = positionEnemy & 0xFF00;
	y = y >> 8;
	x = x - xPawns;
	y = y - yPawns;
	if(x == 0 && y == -33) {
		return true;
	} else {
		return false;
	}
}

bool checkPlayerNearUnder(int xPawns, int yPawns, int positionEnemy) { // quando il nemico è sotto di me
	x = positionEnemy & 0xFF;
	y = positionEnemy & 0xFF00;
	y = y >> 8;
	x = x - xPawns;
	y = y - yPawns;
	if(x == 0 && y == 33) {
		return true;
	} else {
		return false;
	}
}

bool checkPlayerNearRight(int xPawns, int yPawns, int positionEnemy) { // quando il nemico è alla mia destra
	x = positionEnemy & 0xFF;
	y = positionEnemy & 0xFF00;
	y = y >> 8;
	x = x - xPawns;
	y = y - yPawns;
	if(x == +33 && y == 0) {
		return true;
	} else {
		return false;
	}
}

bool checkRightTmp(int move, int enemy) {
	x = move & 0xFF;
	y = move & 0xFF00;
	y = y >> 8;
	return checkPlayerNearRight(x, y, enemy);
}

bool checkPlayerNearLeft(int xPawns, int yPawns, int positionEnemy) { // quando il nemico è alla mia sinistra
	x = positionEnemy & 0xFF;
	y = positionEnemy & 0xFF00;
	y = y >> 8;
	x = x - xPawns;
	y = y - yPawns;
	if(x == -33 && y == 0) {
		return true;
	} else {
		return false;
	}
}

bool checkLeftTmp(int move, int enemy) {
	x = move & 0xFF;
	y = move & 0xFF00;
	y = y >> 8;
	return checkPlayerNearLeft(x, y, enemy);
}

bool checkPlayerNearOverTmp(int move, int enemy) {
	x = move & 0xFF;
	y = move & 0xFF00;
	y = y>> 8;
	return checkPlayerNearOver(x, y, enemy);
}

bool checkPlayerNearUnderTmp(int move, int enemy) {
	x = move & 0xFF;
	y = move & 0xFF00;
	y = y>> 8;
	return checkPlayerNearUnder(x, y, enemy);
}
bool checkPlayerNearDirectionTmp(int move, int enemy, int direction) {
switch(direction){
		case DIRECTION_DOWN:
			return checkPlayerNearUnderTmp(move, enemy);
		case DIRECTION_UP:
			return checkPlayerNearOverTmp(move, enemy);
	}
	return false;
}

bool checkWallsTmpInDirection(int move, int enemy, int direction) {
	switch(direction){
		case DIRECTION_DOWN:
			return checkWallsUnderTmp(move,enemy);
		case DIRECTION_UP:
			return checkWallsOverTmp(move,enemy);
	}
	return false;
}

//----------------------------------------------over
bool checkWallsOverTmp(int move, int enemy) {
	xCoord = move & 0xFF;
	yCoord = move & 0xFF00;
	yCoord = yCoord >> 8;
	if(!checkPlayerNearOverTmp(move, enemy)) {
		return checkWallsOver(xCoord, yCoord);
	} else {
		return checkWallsOver(xCoord, yCoord-33);
	}
}

bool checkWallsOver(int xPawns, int yPawns){
	if(yPawns <= 33+UPPERSPACE){
		return true;
	}
	for(k = 0; k<globalWalls.index; k++){
		x = globalWalls.wallsList[k] & 0xF0000;
		vertical = x == 0;
		
		if(vertical) {
			continue;
		}
		x = globalWalls.wallsList[k] & 0xFF;
		y = globalWalls.wallsList[k] & 0xFF00;
		y = y >> 8;
		x = (x+61) - xPawns;
		y = yPawns - y;
		if(y <= 33 && x <= 55 && x >= 0 && y >= 0){ //33 perché è la distanza tra il walls.wallsList e l'omino. 55 perché si suppone che il walls.wallsList sia lungo di 61 e che la distanza massima sia si 55 tra l'inizio dell'omino
				return true;
			}
	}
	return false;
}

//----------------------------------------------under
bool checkWallsUnderTmp(int move, int enemy) {
	xCoord = move & 0xFF;
	yCoord = move & 0xFF00;
	yCoord = yCoord >> 8;
	if(checkPlayerNearUnderTmp(move, enemy)) {
		return checkWallsUnder(xCoord, yCoord+33);
	} else {
		return checkWallsUnder(xCoord, yCoord);
	}
}
bool checkWallsUnder(int xPawns, int yPawns){
	if(yPawns >= 233){
		return true;
	}
	for(k		= 0; k<globalWalls.index; k++){
		x = globalWalls.wallsList[k] & 0xF0000;
		vertical = x == 0 ? true : false;
		if(vertical) {
			continue;
		}
		x = globalWalls.wallsList[k] & 0xFF;
		y = globalWalls.wallsList[k] & 0xFF00;
		y = y >> 8;
		x = xPawns - x;
		y = y - yPawns;
			if(y <= 33 && x <= 55 && x >= 0 && y >= 0){ //33 perché è la distanza tra il walls.wallsList e l'omino. 55 perché si suppone che il walls.wallsList sia lungo di 61 e che la distanza massima sia si 55 tra l'inizio dell'omino
				return true;
			}
	}
	return false;
}	

//----------------------------------------------left
bool checkWallsLeft(int xPawns, int yPawns){
	if(xPawns <= 33){
		return true;
	}
	for(k		= 0; k<globalWalls.index; k++){
		x = globalWalls.wallsList[k] & 0xF0000;
		vertical = x == 0;
		if(!vertical) {
			continue;
		}
		x = globalWalls.wallsList[k] & 0xFF;
		y = globalWalls.wallsList[k] & 0xFF00;
		y = y >> 8;
		y = (y+61) - yPawns;
		x = xPawns - x;
			if(y <= 55 && x <= 33 && x >= 0 && y >= 0){
				return true;
			}
	}
	return false;
}

//----------------------------------------------right
bool checkWallsRight(int xPawns, int yPawns){
	if(xPawns >= 193){
		return true;
	}
	for(k		= 0; k<globalWalls.index; k++){	
		x = globalWalls.wallsList[k] & 0xF0000;
		vertical = x == 0;
		if(!vertical) {
			continue;
		}
		x = globalWalls.wallsList[k] & 0xFF;
		y = globalWalls.wallsList[k] & 0xFF00;
		y = y >> 8;
		y = (y+61) - yPawns;
		x = x - xPawns;
		if(y <= 55 && x <= 33 && x >= 0 && y >= 0){
				return true;
			}
	}
	return false;
}



void markMoves(int lastMove, int*arrOfYellow, int* size, int positionEnemy){
	int xPawns;
	int yPawns;
	int result = 0;
	*size = 0;
	user = lastMove >> 24;
	xPawns = lastMove & 0xFF;
	yPawns = lastMove & 0xFF00;
	yPawns = yPawns >> 8;
	
	if(!checkWallsOver(xPawns,yPawns) && !checkPlayerNearOver(xPawns, yPawns, positionEnemy)){
		GUI_Text(xPawns, (yPawns-33), (uint8_t *) " ", White, Yellow);
		result = (yPawns-33) << 8;
		result = result + xPawns;
		arrOfYellow[*size] =  result;
		*size = *size + 1;
	}
	if(!checkWallsUnder(xPawns,yPawns) && !checkPlayerNearUnder(xPawns, yPawns, positionEnemy)){
		GUI_Text(xPawns, (yPawns+33), (uint8_t *) " ", White, Yellow);
		result = (yPawns+33) << 8;
		result = result + xPawns;
		arrOfYellow[*size] =  result;
		*size = *size + 1;
	} 
	if(!checkWallsRight(xPawns,yPawns) && !checkPlayerNearRight(xPawns, yPawns, positionEnemy)){
		GUI_Text((xPawns+33), yPawns, (uint8_t *) " ", White, Yellow);
		result = yPawns << 8;
		result = result + (xPawns+33);
		arrOfYellow[*size] =  result;
		*size = *size + 1;
	}
	if(!checkWallsLeft(xPawns,yPawns) && !checkPlayerNearLeft(xPawns, yPawns, positionEnemy)){
		GUI_Text((xPawns-33), yPawns, (uint8_t *) " ", White, Yellow);
		result = yPawns << 8;
		result = result + (xPawns-33);
		arrOfYellow[*size] =  result;
		*size = *size + 1;
	}
  if(checkPlayerNearOver(xPawns,yPawns, positionEnemy) && !checkWallsOver(xPawns,yPawns)){
		x = positionEnemy & 0xFF;
		y = positionEnemy & 0xFF00;
		y = y >> 8;
		if(!checkWallsOver(xPawns,(yPawns-33))) {
			x = positionEnemy & 0xFF;
			y = positionEnemy & 0xFF00;
			y = y >> 8;
			GUI_Text(x, (y-33), (uint8_t *) " ", White, Yellow);
			result = (y-33) << 8;
			result = result + x;
			arrOfYellow[*size] =  result;
			*size = *size + 1;
		}
	}
	if(checkPlayerNearUnder(xPawns,yPawns, positionEnemy) && !checkWallsUnder(xPawns,yPawns)){
		x = positionEnemy & 0xFF;
		y = positionEnemy & 0xFF00;
		y = y >> 8;
		if(!checkWallsUnder(xPawns,(yPawns+33))) {
			x = positionEnemy & 0xFF;
			y = positionEnemy & 0xFF00;
			y = y >> 8;
			GUI_Text(x, (y+33), (uint8_t *) " ", White, Yellow);
			result = (y+33) << 8;
			result = result + x;
			arrOfYellow[*size] =  result;
			*size = *size + 1;
		}
	}
			
}

bool checkWall(int position, int direction){
	int xP = position & 0xFF;
	int yP = position & 0xFF00;
	yP = yP >> 8;
	switch(direction){
		case DIRECTION_RIGHT:
			return checkWallsRight(xP, yP);
		case DIRECTION_DOWN:
			return checkWallsUnder(xP, yP);
		case DIRECTION_LEFT:
			return checkWallsLeft(xP, yP);
		case DIRECTION_UP:
			return checkWallsOver(xP, yP);
	}
	return false;
}

void removeMarkedMoves(int*arrOfYellow, int* size){
	int i = 0;
	int x;
	int y;
	for(i=0; i < *size; i++){
		x = arrOfYellow[i] & 0xFF;
		y = arrOfYellow[i] & 0xFF00;
		y = y >> 8;
 		GUI_Text(x,y, (uint8_t *) "  ", White, White);
	}
	
	*size = 0;
}

bool pawsArrived(int position){
	int user = position >> 24;
	int xP = position & 0xFF;
	int yP = position & 0xFF00;
	yP = yP >> 8;
	if(position == 0){
		return false;
	}
	if(user == 0){
		if(yP <= 33+UPPERSPACE){
			return true;
		}
	} else if(user == 1){
			if(yP >= 233){
			return true;
		}
	}
	return false;
}

void endGame(int position) {
	if(pawsArrived(position)){
		disable_timer(0);
		disable_timer(1);
		NVIC_DisableIRQ(EINT1_IRQn);
		NVIC_DisableIRQ(EINT2_IRQn);
		NVIC_DisableIRQ(EINT0_IRQn);
		LCD_Clear(White);
		GUI_Text(48, 160, (uint8_t *) "END GAME, PRESS RESET", Black, White);
		disable_RIT();
	}
}


void pawnMoved() {
	if(globalGameInfo.current_turn_player == 0) {
		if(m != 0) {
			movePawns(m, true);
			m = 0;
			movePawns(globalGameInfo.last_move_p0, false);
		}
	} else {
		if(m != 0) {
			movePawns(m, true);
			m = 0;
			movePawns(globalGameInfo.last_move_p1, false);
		}
	}
}
