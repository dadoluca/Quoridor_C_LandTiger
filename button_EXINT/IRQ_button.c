#include "button.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../walls/walls.h"
#include "../pawns/pawns.h"
#include "../timer/timer.h"
#include "../move/move.h"
volatile int i = 0;
volatile int j = 0;
volatile int count = 0;
volatile int count2 = 0;
volatile int h = 0;
volatile int h2 = 0;
int oldMove1;
int oldMove0;
extern int currentPlayer;

extern int downKey1;
extern int downKey2;
extern int downKey0;

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
	downKey0 = 1;
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 22);     /* GPIO pin selection */
	downKey1 = 1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4 &= ~(1 << 24);     /* GPIO pin selection */
	downKey2 = 1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */  
}

void buttonEint0(){
	LCD_Clear(White);
	/*
	*
	*	CASELLE INFORMAZIONI
	*
	*/
	LCD_DrawLine(5, 275, 75, 275 , Black);
	LCD_DrawLine(5, 275, 5, 315 , Black);
	GUI_Text(10, 280, (uint8_t *) "P1 Wall", Black, White);
	GUI_Text(35, 300, (uint8_t *) "8", Black, White);
	LCD_DrawLine(75, 275, 75, 315 , Black);
	LCD_DrawLine(5, 315, 75, 315 , Black);

	LCD_DrawLine(85, 275, 155, 275 , Black);
	LCD_DrawLine(85, 275, 85, 315 , Black);
	GUI_Text(105, 287, (uint8_t *) "20 s", Black, White);
	LCD_DrawLine(85, 315, 155, 315 , Black);
	LCD_DrawLine(155, 275, 155, 315 , Black);

	LCD_DrawLine(165, 275, 235, 275 , Black);
	LCD_DrawLine(165, 275, 165, 315 , Black);
	GUI_Text(170, 280, (uint8_t *) "P2 Wall", Black, White);
	GUI_Text(195, 300, (uint8_t *) "8", Black, White);
	LCD_DrawLine(165, 315, 235, 315 , Black);
	LCD_DrawLine(235, 275, 235, 315 , Black);
	
	/*
	*
	*	SCACCHIERA
	*
	*/
	i = 0;
	count = 0; 
	count2 = 0;
	h = 260;
	h2 = 0;
	for(j = 0; j < 7; j++){
		count = 0;
		count2 = 0;
		h = h - 5;
		h2 = h - 28;
		for(i = 0; i < 7; i++){
			count = count + 5;
			count2 = count + 28;
			LCD_DrawLine(count, h, count2, h, Black);
			LCD_DrawLine(count, h2, count2, h2, Black);
			LCD_DrawLine(count, h, count, h2, Black);
			LCD_DrawLine(count2, h, count2, h2, Black); 		
			count = count2;
		}
		h = h2;
	}
	/*
	*
	*	PEDINE
	*
	*/
	GUI_Text(113, 233, (uint8_t *) " ", White, Red);
	GUI_Text(113, 35, (uint8_t *) " ", White, Blue);
	initialPosition();
	
	/*
	movePawns(0x100C831,0);
	movePawns(0,0x000c831);

	GUI_Text(16, 233, (uint8_t *) " ", White, Red);
	GUI_Text(49, 200, (uint8_t *) " ", White, Blue);

	
	movePawns(0x100C831, 0);
	movePawns(0, 0x000c831);
  markMoves(0x100C831);
  */
	
	//init e enable dei timer per il conteggio dei 20 secondi
	init_timer(0, 0x017D7840);
	init_timer(1, 0x1F4ADD40);	/* 20 sec */ 
	enable_timer(1);
	enable_timer(0);

	//draw_wall_wrapper(0x110C264, false);
	//markMoves(0x0000E971);

	globalWalls.index = 0;

	//draw_wall_wrapper(0x110C264, false);
	//draw_wall_wrapper(0x011DF05, false);
	//draw_wall_wrapper(0x011BE05, false);
	
	//markMoves(oldMove0);
	//Enable del bottone key1, pulizia pending
	LPC_SC->EXTINT &= (1 << 1);
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
}

void buttonEint1(){
	int move;
	int move2;
	int xW;
	int yW;
	
	bool up = true;
	
	removeMarkedMoves(blocchi_gialli, &size);
	pawnMoved();
	
	if(finishedWalls(currentPlayer)){
		writeWarningMessage(currentPlayer, false);
	}
	
	defaultCenteredWall = currentPlayer==1 ? 0x1119D68 : 0x0119D68;	
	
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


