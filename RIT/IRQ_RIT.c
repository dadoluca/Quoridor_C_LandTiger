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
void EINT0_Actions();
void KEY1_Actions();
void KEY2_Actions();
void confirmEndTurn();
void joystickMove(int direction);
extern void initGame();

extern struct GameInfo globalGameInfo;
int id = 1;
volatile int down_key1=0;
volatile int down_key2=0;
volatile int down_eint0=0;
int current_default_beginning_wall;
int lastMoveTmp;
int m;
int lastWallMoveTmp;
extern bool reset;
int yCoor;
extern bool reset;
int array_possible_moves[4];
int size;
int currentWallMoveTmp=0;

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
				 joystickMove(DIRECTION_UP);
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
					joystickMove(DIRECTION_RIGHT);
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	//----------------------------------------- LEFT
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
					joystickMove(DIRECTION_LEFT);
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
				 joystickMove(DIRECTION_DOWN);
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
				confirmEndTurn();
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
					EINT0_Actions();
				
					//enable key1 for wall positioning
					LPC_SC->EXTINT &= (1 << 1);					/* clear pending interrupt       */
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

	/*KEY1 debouncing management ------------------------------------------------------------------*/
	if(down_key1>1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* ----------------------------- KEY1 pressed */
			reset_RIT();// to manage the next 50 millisecondi of polling
			switch(down_key1){				
				case 2://------------my button code	
					KEY1_Actions();
				
					//enable key2 for wall rotation
					LPC_SC->EXTINT &= (1 << 2);							/* clear pending interrupt         */
					NVIC_ClearPendingIRQ(EINT2_IRQn);
					NVIC_EnableIRQ(EINT2_IRQn);							/* enable Button KEY2 interrupts */
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
	
	/*KEY2 debouncing management ------------------------------------------------------------------*/
	if(down_key2>1){ 
		if((LPC_GPIO2->FIOPIN & (1 << 12)) == 0){	/* ----------------------------- KEY2 pressed */
			reset_RIT();// to manage the next 50 millisecondi of polling
			switch(down_key2){				
				case 2://------------my button code	
					KEY2_Actions();
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

/*EINT0 game actions ------------------------------------------------------------------*/
void EINT0_Actions(){
	initGame(); //init global variables and draw board
					
	//init and enable the timers for counting the 20 seconds
	init_timer(0, 0x017D7840);	/* 1 sec  */
	init_timer(1, 0x1F4ADD40);	/* 20 sec */ 
	enable_timer(1);						
	enable_timer(0);
}

/*KEY1 game actions ------------------------------------------------------------------*/
void KEY1_Actions(){
	removeMarkedMoves(array_possible_moves, &size);
	pawnMoved();
	
	if(finishedWalls(globalGameInfo.current_turn_player)){
		writeWarningMessage(globalGameInfo.current_turn_player, false);
	}
	
	current_default_beginning_wall = getNextPlaceableWall();	
	draw_wall_wrapper(current_default_beginning_wall, false);

	globalGameInfo.current_move_mode = WALL_MODE;
}

/*KEY2 game actions ------------------------------------------------------------------*/
void KEY2_Actions(){
	lastWallMoveTmp = lastWallMoveTmp == 0 ? current_default_beginning_wall : lastWallMoveTmp;
	updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
	
	if(checkPosition(lastWallMoveTmp)){
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
		draw_wall_wrapper(lastWallMoveTmp, true);
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
		draw_wall_wrapper(lastWallMoveTmp, false); 	
	}	else {
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
	}
}



void joystickMove(int direction){
	bool cond;
	bool horizontal = direction==DIRECTION_RIGHT || direction==DIRECTION_LEFT;
	
	if(globalGameInfo.current_turn_player == PLAYER_1){
	//----------------------------- PLAYER_1
		if(globalGameInfo.current_move_mode == TOKEN_MODE){
		//-------------- TOKEN_MODE
			lastMoveTmp = m == 0 ? globalGameInfo.last_move_p1 : m;
			cond = horizontal ? !checkRightTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0) : true;
 			if(!checkWall(globalGameInfo.last_move_p1, direction) && cond){
				m = globalGameInfo.last_move_p1;
				updateMovePlayer(&m,PLAYER_1);
				updateMovePlayerWall(&m,true);
				if(horizontal){
					updateMoveInDirection(&m,direction);
					movePawns(lastMoveTmp, true);
					markMoves(globalGameInfo.last_move_p1, array_possible_moves, &size, globalGameInfo.last_move_p0);
					movePawns(m,false);
				}
				else if(!checkPlayerNearDirectionTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0, direction) && !checkWallsTmpInDirection(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0,direction)) {
					updateMoveInDirection(&m,direction);//calculates the movment based on direction
					if(m != globalGameInfo.last_move_p1) {
						movePawns(lastMoveTmp, true);
					}
				} else if(checkPlayerNearDirectionTmp(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0, direction) && !checkWallsTmpInDirection(globalGameInfo.last_move_p1, globalGameInfo.last_move_p0,direction)) {
					updateMoveUpPlayer(&m);
					if(m != globalGameInfo.last_move_p1) {
						movePawns(globalGameInfo.last_move_p1, true);
					}
				} else { 
					m = lastMoveTmp;
				}
				if(m != globalGameInfo.last_move_p1 &&!horizontal) {
					markMoves(globalGameInfo.last_move_p1, array_possible_moves, &size, globalGameInfo.last_move_p0);
					movePawns(m, false);
				}
			}
		}else{
		//-------------- WALL_MODE
			lastWallMoveTmp = lastWallMoveTmp == 0 ? current_default_beginning_wall : lastWallMoveTmp;
			currentWallMoveTmp = lastWallMoveTmp;
			updateMovePlayer(&currentWallMoveTmp,PLAYER_1);
			updateMoveInDirection(&currentWallMoveTmp,direction);//calculates the movment based on direction
			if(checkPosition(currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){
				draw_wall_wrapper(lastWallMoveTmp,true); //deletes old wall
				draw_wall_wrapper(currentWallMoveTmp,false);//draws new wall
				lastWallMoveTmp = currentWallMoveTmp;
			}
		}
	} else{
	//----------------------------- PLAYER_0
		if(globalGameInfo.current_move_mode == TOKEN_MODE){
		//-------------- TOKEN_MODE
			lastMoveTmp = m == 0 ? globalGameInfo.last_move_p0 : m;
			cond = horizontal ?  !checkRightTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1) : true;
			if(!checkWall(globalGameInfo.last_move_p0, direction) && cond){
				m = globalGameInfo.last_move_p0;
				updateMovePlayer(&m,PLAYER_0);
				updateMovePlayerWall(&m,true); 
				if(horizontal){
					updateMoveInDirection(&m,direction);
					movePawns(lastMoveTmp, true);
					markMoves(globalGameInfo.last_move_p0, array_possible_moves, &size, globalGameInfo.last_move_p1);
					movePawns(m,false);
				}
				else if(!checkPlayerNearDirectionTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1,direction) && !checkWallsTmpInDirection(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1, direction)) {
					updateMoveInDirection(&m,direction);//calculates the movment based on direction
					if(m != globalGameInfo.last_move_p0) {
					movePawns(lastMoveTmp, true);
					}else if(checkPlayerNearDirectionTmp(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1,direction) && !checkWallsTmpInDirection(globalGameInfo.last_move_p0, globalGameInfo.last_move_p1, direction)){
						updateMoveUpPlayer(&m);
						if(m != globalGameInfo.last_move_p0) {
							movePawns(globalGameInfo.last_move_p0, true);
						}
					} else {
						m = lastMoveTmp;
					}
					if(m != globalGameInfo.last_move_p0) {
						markMoves(globalGameInfo.last_move_p0, array_possible_moves, &size, globalGameInfo.last_move_p1);
						movePawns(m, false);
					}
				}
			}
		}else{
		//-------------- WALL_MODE
			lastWallMoveTmp = lastWallMoveTmp == 0 ? current_default_beginning_wall : lastWallMoveTmp;
			currentWallMoveTmp = lastWallMoveTmp;
			updateMovePlayer(&currentWallMoveTmp,PLAYER_0);
			updateMoveInDirection(&currentWallMoveTmp,direction);//calculates the movment based on direction
			if(checkPosition(currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p0, currentWallMoveTmp) && checkPermanentPositionWall(globalGameInfo.last_move_p1, currentWallMoveTmp)){//TODO: sostituire true con il controllo sul posizionamento del muro
				draw_wall_wrapper(lastWallMoveTmp,true); //deletes old wall
				draw_wall_wrapper(currentWallMoveTmp,false);//draws new wall
				lastWallMoveTmp = currentWallMoveTmp;
			}
		}
	}
}

void confirmEndTurn(){
	removeMarkedMoves(array_possible_moves, &size);
	reset = true;	
	reset_timer(1);
	enable_timer(1);
	if(globalGameInfo.current_move_mode == TOKEN_MODE){//pawn
		if(globalGameInfo.current_turn_player == PLAYER_0) {//player 0
			globalGameInfo.last_move_p0 = m != 0 ? m : globalGameInfo.last_move_p0;
			movePawns(globalGameInfo.last_move_p0,false);
			markMoves(globalGameInfo.last_move_p1, array_possible_moves, &size, globalGameInfo.last_move_p0);
		} else { //player 1
			globalGameInfo.last_move_p1 = m != 0 ? m : globalGameInfo.last_move_p1;
			movePawns(globalGameInfo.last_move_p1,false);
			markMoves(globalGameInfo.last_move_p0, array_possible_moves, &size, globalGameInfo.last_move_p1);
		}
	}
	else{//wall
		globalGameInfo.last_move_p0 = m != 0 ? m : globalGameInfo.last_move_p0;
		globalGameInfo.last_move_p1 = m != 0 ? m : globalGameInfo.last_move_p1;
		if(lastWallMoveTmp != 0){
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		} else {
			lastWallMoveTmp = lastWallMoveTmp == 0 ? current_default_beginning_wall : lastWallMoveTmp;
			globalWalls.wallsList[globalWalls.index] = lastWallMoveTmp;
			globalWalls.index = globalWalls.index + 1;
		}
		lastWallMoveTmp=0;
		if(globalGameInfo.current_turn_player == PLAYER_0) {
			markMoves(globalGameInfo.last_move_p1, array_possible_moves, &size, globalGameInfo.last_move_p0);
		} else {
			markMoves(globalGameInfo.last_move_p0, array_possible_moves, &size, globalGameInfo.last_move_p1);
		}
		//controllo per fine muri
		updateCountWalls(globalGameInfo.current_turn_player);
		writeWarningMessage(globalGameInfo.current_turn_player, true);		
	}
	globalGameInfo.current_turn_player = globalGameInfo.current_turn_player==PLAYER_1 ? PLAYER_0 : PLAYER_1;
	globalGameInfo.current_move_mode=TOKEN_MODE;
	NVIC_DisableIRQ(EINT2_IRQn);	//Disable del bottone key2
	//Abilito il key1, con il clear del pending
	LPC_SC->EXTINT &= (1 << 1);
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
