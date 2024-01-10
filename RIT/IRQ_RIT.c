/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../pawns/pawns.h"
#include "../move/move.h"
#include "../timer/timer.h"
#include "../walls/walls.h"
#include "../board/board.h"
#include "../button_EXINT/button.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int id = 1;

volatile int downKey1=0;
volatile int downKey2=0;
volatile int downKey0=0;

extern int oldMove1;
extern int oldMove0;
int lastMoveTmp;
int m;
extern int defaultCenteredWall;
int lastWallMoveTmp;
extern bool reset;
int yCoor;


extern int currentPlayer; 	
extern bool reset;
extern int wallMode;
extern int blocchi_gialli[4];
extern int size;
extern int currentPlayer;
int currentWallMoveTmp=0;


void confirmMove(){
	removeMarkedMoves(blocchi_gialli, &size);
	reset = true;	
	reset_timer(1);
	enable_timer(1);
	if(wallMode == 0){//pawn
		if(currentPlayer == 0) {//player 0
			oldMove0 = m != 0 ? m : oldMove0;
			movePawns(oldMove0,false);
			markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
		} else { //player 1
			oldMove1 = m != 0 ? m : oldMove1;
			movePawns(oldMove1,false);
			markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
		}
	}
	else{//wall
		oldMove0 = m != 0 ? m : oldMove0;
		oldMove1 = m != 0 ? m : oldMove1;
		if(lastWallMoveTmp != 0){
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		} else {
			lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		}
		lastWallMoveTmp=0;
		if(currentPlayer == 0) {
			markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
		} else {
			markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
		}
		//controllo per fine muri
		updateCountWalls(currentPlayer);
		writeWarningMessage(currentPlayer, true);		
	}
	currentPlayer = currentPlayer==1 ? 0 : 1;
	wallMode=0;
	NVIC_DisableIRQ(EINT2_IRQn);	//Disable del bottone key2
	//Abilito il key1, con il clear del pending
	LPC_SC->EXTINT &= (1 << 1);
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
}

void RIT_IRQHandler (void)
{	
	static int up=0;
	static int right=0;
	static int left=0;	
	static int downJ=0;
	static int select=0;

	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){//----------------------------------------- UP	
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1:
				if(currentPlayer == 1){//player 1
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove1 : m;
 					if(!checkWall(oldMove1, 3)){
						m = oldMove1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearOverTmp(oldMove1, oldMove0) && !checkWallsOverTmp(oldMove1, oldMove0)) {
							updateMoveUp(&m);
							if(m != oldMove1) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearOverTmp(oldMove1, oldMove0) && !checkWallsOverTmp(oldMove1, oldMove0)) {
							updateMoveUpPlayer(&m);
							if(m != oldMove1) {movePawns(oldMove1, true);}
						} else { m = lastMoveTmp;}
						
						if(m != oldMove1) {
							markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
							movePawns(m, false);
						}
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveUp(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove1) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{//player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove0 : m;
					if(!checkWall(oldMove0, 3)){
						m = oldMove0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearOverTmp(oldMove0, oldMove1) && !checkWallsOverTmp(oldMove0, oldMove1)) {
							updateMoveUp(&m);
							if(m != oldMove0) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearOverTmp(oldMove0, oldMove1) && !checkWallsOverTmp(oldMove0, oldMove1)){
							updateMoveUpPlayer(&m);
							if(m != oldMove0) {movePawns(oldMove0, true);}
						} else { m = lastMoveTmp;}
						if(m != oldMove0) {
							markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
							movePawns(m, false);
						}
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveUp(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove0) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){//----------------------------------------- RIGHT	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
				if(currentPlayer == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove1 : m;
					if(!checkWall(oldMove1, 0) && !checkRightTmp(oldMove1, oldMove0)){
						m = oldMove1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						updateMoveRight(&m);
						movePawns(lastMoveTmp, true);
						markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveRight(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove1) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove0 : m;
					if(!checkWall(oldMove0, 0) && !checkRightTmp(oldMove0, oldMove1)){
						m = oldMove0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						updateMoveRight(&m);
						movePawns(lastMoveTmp, true);
						markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveRight(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove0) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
				}
				break;
			default:
				break;
		}
	}
	}else{
			right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	//----------------------------------------- LEFT
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				if(currentPlayer == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove1 : m;
					if(!checkWall(oldMove1, 2) && !checkLeftTmp(oldMove1, oldMove0)){
						m = oldMove1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						updateMoveLeft(&m);
						movePawns(lastMoveTmp, true);
						markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveLeft(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove1) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{//player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove0 : m;
					if(!checkWall(oldMove0, 2) && !checkLeftTmp(oldMove0, oldMove1)){
						m = oldMove0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						updateMoveLeft(&m);
						movePawns(lastMoveTmp, true);
						markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
						movePawns(m,false);
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveLeft(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove0) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	//----------------------------------------- DOWN
		/* Joytick DOWN pressed */
		downJ++;
		switch(downJ){
			case 1:
				if(currentPlayer == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove1 : m;
					if(!checkWall(oldMove1, 1)){
						m = oldMove1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearUnderTmp(oldMove1, oldMove0) && !checkWallsUnderTmp(oldMove1, oldMove0)) {
							updateMoveDown(&m);
							if(m != oldMove1) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearUnderTmp(oldMove1, oldMove0) && !checkWallsUnderTmp(oldMove1, oldMove0)) {
							updateMoveDownPlayer(&m);
							if(m != oldMove1) {movePawns(oldMove1, true);}
						} else { m = lastMoveTmp;}
						if(m != oldMove1) {
							markMoves(oldMove1, blocchi_gialli, &size, oldMove0);
							movePawns(m, false);
						}
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveDown(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove1) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{ //Player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? oldMove0 : m;
					if(!checkWall(oldMove0, 1)){
						m = oldMove0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearUnderTmp(oldMove0, oldMove1) && !checkWallsUnderTmp(oldMove0, oldMove1)) {
							updateMoveDown(&m);
							if(m != oldMove0) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearUnderTmp(oldMove0, oldMove1) && !checkWallsUnderTmp(oldMove0, oldMove1)) {
							updateMoveDownPlayer(&m);
							if(m != oldMove0) {movePawns(oldMove0, true);}
						} else { m = lastMoveTmp;}
						if(m != oldMove0) {
							markMoves(oldMove0, blocchi_gialli, &size, oldMove1);
							movePawns(m,false);
						}
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveDown(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, oldMove0) && checkPermanentPositionWall(oldMove0, currentWallMoveTmp) && checkPermanentPositionWall(oldMove1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				}

				break;
			default:
				break;
		}
	}
	else{
			downJ=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	//----------------------------------------- SELECT
		/* Joytick SELECT pressed */
		select++;
		switch(select){
			case 1:
				confirmMove();
				//controllo di fine partita
				endGame(m);
				m = 0;
				break;
			case 100:
				break;
			default:
				break;
		}
	}
	else{
			select=0;
	}
	
	/* button management */
	if(downKey1>1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			reset_RIT();// voglio controllare i successivi 50 millisecondi
			switch(downKey1){				
				case 2:
					buttonEint1();
					break;
				default:
					break;
			}
			downKey1++;
		}else {	/* button released */
			downKey1=0;
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	} else {
		if(downKey1 == 1)
			downKey1++;
	}
	if(downKey2>1){ 
		if((LPC_GPIO2->FIOPIN & (1 << 12)) == 0){	/* KEY2 pressed */
			reset_RIT();// voglio controllare i successivi 50 millisecondi
			switch(downKey2){				
				case 2:
					buttonEint2();
					break;
				default:
					break;
			}
			downKey2++;
		}else {
			downKey2=0;
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 24);
		}
	} else {
		if(downKey2 == 1)
			downKey2++;
	}
	
	if(downKey0>1){ 
		if((LPC_GPIO2->FIOPIN & (1 << 10)) == 0){	/* INT0 pressed */
			reset_RIT();// voglio controllare i successivi 50 millisecondi
			switch(downKey0){				
				case 2:
					buttonEint0();
					break;
				default:
					break;
			}
			downKey0++;
		}else {
			downKey0=0;
			NVIC_EnableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4 |= (1 << 20);
		}
	} else {
		if(downKey0 == 1)
			downKey0++;
	}
	
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
