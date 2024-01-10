#include "button.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../walls/walls.h"
#include "../pawns/pawns.h"
#include "../timer/timer.h"
#include "../move/move.h"
#include "../game/game.h"

extern struct GameInfo globalGameInfo;

extern int down_key1;
extern int down_key2;
extern int down_eint0;

int defaultCenteredWall;
extern int lastWallMoveTmp;
 
//-------gialli
extern int blocchi_gialli[4];
extern int size;
//-------gialli
int wallMode;


void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	enable_RIT();										/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 20);
	down_eint0 = 1;
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 22);     /* GPIO pin selection */
	down_key1 = 1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 24);     /* GPIO pin selection */
	down_key2 = 1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */  
}


void buttonEint1(){
	int xW;
	int yW;
	
	bool up = true;
	
	removeMarkedMoves(blocchi_gialli, &size);
	pawnMoved();
	
	if(finishedWalls(globalGameInfo.current_turn_player)){
		writeWarningMessage(globalGameInfo.current_turn_player, false);
	}
	
	defaultCenteredWall = globalGameInfo.current_turn_player==1 ? 0x1119D68 : 0x0119D68;	
	
	while(!checkPosition(defaultCenteredWall, 0x0)){
		xW = defaultCenteredWall & 0xFF;
		yW = defaultCenteredWall & 0xFF00;
		yW = yW >> 8;
		if(UPPERSPACE > yW-33 || !up){
			updateMoveDown(&defaultCenteredWall);
			up = false;
		} else if(up/*yW + 33 > 233*/){
			updateMoveUp(&defaultCenteredWall);
		}
	}
	
	draw_wall_wrapper(defaultCenteredWall, false);
	
	
	
	//enable_RIT();										/* enable RIT to count 50ms				 */
	
	//Abilito il bottone key2, con clear del pending
	LPC_SC->EXTINT &= (1 << 2);
	NVIC_ClearPendingIRQ(EINT2_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);

	wallMode = 1;
}

void buttonEint2(){
	lastWallMoveTmp = lastWallMoveTmp == 0 ? defaultCenteredWall : lastWallMoveTmp;
	updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
	if(checkPosition(lastWallMoveTmp, 0x0)){
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
		draw_wall_wrapper(lastWallMoveTmp, true);
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
		draw_wall_wrapper(lastWallMoveTmp, false); 	
	} else {
		updateMoveVerticalHorizontal(&lastWallMoveTmp, true);
	}
}


