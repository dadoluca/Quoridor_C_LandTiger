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
#include "../game/game.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern struct GameInfo globalGameInfo;
extern void initGame();


int id = 1;

volatile int down_key1=0;
volatile int down_key2=0;
volatile int down_eint0=0;


int lastMoveTmp;
int m;
extern int defaultCenteredWall;
int lastWallMoveTmp;
extern bool reset;
int yCoor;

	
extern bool reset;
extern int wallMode;
extern int blocchi_gialli[4];
extern int size;
int currentWallMoveTmp=0;


void confirmMove(){
	removeMarkedMoves(blocchi_gialli, &size);
	reset = true;	
	reset_timer(1);
	enable_timer(1);
	if(wallMode == 0){//pawn
		if(globalGameInfo.current_turn_player == 0) {//player 0
			globalGameInfo.last_move_p0 = m != 0 ? m : globalGameInfo.last_move_p0;
			movePawns(globalGameInfo.last_move_p0,false);
			markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
		} else { //player 1
			globalGameInfo.last_move_p1 = m != 0 ? m : globalGameInfo.last_move_p1;
			movePawns(globalGameInfo.last_move_p1,false);
			markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
		}
	}
	else{//wall
		globalGameInfo.last_move_p0 = m != 0 ? m : globalGameInfo.last_move_p0;
		globalGameInfo.last_move_p1 = m != 0 ? m : globalGameInfo.last_move_p1;
		if(lastWallMoveTmp != 0){
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		} else {
			lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		}
		lastWallMoveTmp=0;
		if(globalGameInfo.current_turn_player == 0) {
			markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
		} else {
			markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
		}
		//controllo per fine muri
		updateCountWalls(globalGameInfo.current_turn_player);
		writeWarningMessage(globalGameInfo.current_turn_player, true);		
	}
	globalGameInfo.current_turn_player = globalGameInfo.current_turn_player==1 ? 0 : 1;
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
				if(globalGameInfo.current_turn_player == 1){//player 1
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p1 : m;
 					if(!checkWall(globalGameInfo.last_move_p1, 3)){
						m = globalGameInfo.last_move_p1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearOverTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0) && !checkWallsOverTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)) {
							updateMoveUp(&m);
							if(m != globalGameInfo.last_move_p1) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearOverTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0) && !checkWallsOverTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)) {
							updateMoveUpPlayer(&m);
							if(m != globalGameInfo.last_move_p1) {movePawns(globalGameInfo.last_move_p1, true);}
						} else { m = lastMoveTmp;}
						
						if(m != globalGameInfo.last_move_p1) {
							markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
							movePawns(m, false);
						}
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveUp(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p1) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{//player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p0 : m;
					if(!checkWall(globalGameInfo.last_move_p0, 3)){
						m = globalGameInfo.last_move_p0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearOverTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1) && !checkWallsOverTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)) {
							updateMoveUp(&m);
							if(m != globalGameInfo.last_move_p0) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearOverTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1) && !checkWallsOverTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)){
							updateMoveUpPlayer(&m);
							if(m != globalGameInfo.last_move_p0) {movePawns(globalGameInfo.last_move_p0, true);}
						} else { m = lastMoveTmp;}
						if(m != globalGameInfo.last_move_p0) {
							markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
							movePawns(m, false);
						}
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveUp(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p0) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
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
				if(globalGameInfo.current_turn_player == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p1 : m;
					if(!checkWall(globalGameInfo.last_move_p1, 0) && !checkRightTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)){
						m = globalGameInfo.last_move_p1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						updateMoveRight(&m);
						movePawns(lastMoveTmp, true);
						markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveRight(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p1) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p0 : m;
					if(!checkWall(globalGameInfo.last_move_p0, 0) && !checkRightTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)){
						m = globalGameInfo.last_move_p0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						updateMoveRight(&m);
						movePawns(lastMoveTmp, true);
						markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveRight(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p0) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
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
				if(globalGameInfo.current_turn_player == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p1 : m;
					if(!checkWall(globalGameInfo.last_move_p1, 2) && !checkLeftTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)){
						m = globalGameInfo.last_move_p1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						updateMoveLeft(&m);
						movePawns(lastMoveTmp, true);
						markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
						movePawns(m,false);
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveLeft(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p1) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{//player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p0 : m;
					if(!checkWall(globalGameInfo.last_move_p0, 2) && !checkLeftTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)){
						m = globalGameInfo.last_move_p0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						updateMoveLeft(&m);
						movePawns(lastMoveTmp, true);
						markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
						movePawns(m,false);
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveLeft(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p0) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
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
				if(globalGameInfo.current_turn_player == 1){
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p1 : m;
					if(!checkWall(globalGameInfo.last_move_p1, 1)){
						m = globalGameInfo.last_move_p1;
						updateMovePlayer(&m,1);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearUnderTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0) && !checkWallsUnderTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)) {
							updateMoveDown(&m);
							if(m != globalGameInfo.last_move_p1) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearUnderTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0) && !checkWallsUnderTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0)) {
							updateMoveDownPlayer(&m);
							if(m != globalGameInfo.last_move_p1) {movePawns(globalGameInfo.last_move_p1, true);}
						} else { m = lastMoveTmp;}
						if(m != globalGameInfo.last_move_p1) {
							markMoves(globalGameInfo.last_move_p1, blocchi_gialli, &size, globalGameInfo.last_move_p0);
							movePawns(m, false);
						}
					}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,1);//player a 1
						updateMoveDown(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p1) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
							draw_wall_wrapper(lastWallMoveTmp,true); //cancella muro vecchio
							draw_wall_wrapper(currentWallMoveTmp,false);//disegno nuovo muro
							lastWallMoveTmp = currentWallMoveTmp;
						}
					}
				} else{ //Player 0
					if(wallMode == 0){//pawn
						lastMoveTmp = m == 0 ? globalGameInfo.last_move_p0 : m;
					if(!checkWall(globalGameInfo.last_move_p0, 1)){
						m = globalGameInfo.last_move_p0;
						updateMovePlayer(&m,0);
						updateMovePlayerWall(&m,true);
						if(!checkPlayerNearUnderTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1) && !checkWallsUnderTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)) {
							updateMoveDown(&m);
							if(m != globalGameInfo.last_move_p0) {movePawns(lastMoveTmp, true);}
						} else if(checkPlayerNearUnderTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1) && !checkWallsUnderTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1)) {
							updateMoveDownPlayer(&m);
							if(m != globalGameInfo.last_move_p0) {movePawns(globalGameInfo.last_move_p0, true);}
						} else { m = lastMoveTmp;}
						if(m != globalGameInfo.last_move_p0) {
							markMoves(globalGameInfo.last_move_p0, blocchi_gialli, &size, globalGameInfo.last_move_p1);
							movePawns(m,false);
						}
						}
					}else{//wall
						lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
						currentWallMoveTmp = lastWallMoveTmp;
						updateMovePlayer(&currentWallMoveTmp,0);//player a 0
						updateMoveDown(&currentWallMoveTmp);//move wall up 
						if(checkPosition(currentWallMoveTmp, globalGameInfo.last_move_p0) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
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
	
/*-----------------------------------------------  BUTTONS MANAGEMENT -----------------------------------------------*/
	
	/*EINT0 debouncing management ------------------------------------------------------------------*/
	if(down_eint0>1){ 
		if((LPC_GPIO2->FIOPIN & (1 << 10)) == 0){	/* ----------------------------- EINT0 pressed */
			reset_RIT();// to manage the next 50 millisecondi of polling
			switch(down_eint0){				
				case 2://------------my button code	
					
					initGame(); //init global variables and draw board
					
					//init e enable dei timer per il conteggio dei 20 secondi
					init_timer(0, 0x017D7840);	/* 1 sec  */
					init_timer(1, 0x1F4ADD40);	/* 20 sec */ 
					enable_timer(1);						
					enable_timer(0);
					
					//Enable del bottone key1, pulizia pending
					LPC_SC->EXTINT &= (1 << 1);					/* clear pending interrupt         */
					NVIC_ClearPendingIRQ(EINT1_IRQn);		
					NVIC_EnableIRQ(EINT1_IRQn);					/* enable Button KEY1 interrupts */
							
					break;
				
				default:
					break;
			}
			down_eint0++;
		}else {
			down_eint0=0;
			NVIC_EnableIRQ(EINT0_IRQn);
			LPC_PINCON->PINSEL4 |= (1 << 20);
		}
	} else {
		if(down_eint0 == 1)
			down_eint0++;
	}

	/*KEY debouncing management ------------------------------------------------------------------*/
	if(down_key1>1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* ----------------------------- KEY1 pressed */
			reset_RIT();// to manage the next 50 millisecondi of polling
			switch(down_key1){				
				case 2://------------my button code	
					buttonEint1();
					break;
				default:
					break;
			}
			down_key1++;
		}else {	/* button released */
			down_key1=0;
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 22); /* External interrupt 0 pin selection */
		}
	} else {
		if(down_key1 == 1)
			down_key1++;
	}
	
	/*KEY debouncing management ------------------------------------------------------------------*/
	if(down_key2>1){ 
		if((LPC_GPIO2->FIOPIN & (1 << 12)) == 0){	/* ----------------------------- KEY2 pressed */
			reset_RIT();// to manage the next 50 millisecondi of polling
			switch(down_key2){				
				case 2://------------my button code	
					buttonEint2();
					break;
				default:
					break;
			}
			down_key2++;
		}else {
			down_key2=0;
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 24);
		}
	} else {
		if(down_key2 == 1)
			down_key2++;
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
/*--------------------------------------------- END BUTTONS MANAGEMENT ---------------------------------------------*/

	
  return;
}




/******************************************************************************
**                            End Of File
******************************************************************************/
