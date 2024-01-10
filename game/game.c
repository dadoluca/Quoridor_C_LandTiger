#include <stdio.h>
#include "game.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../walls/walls.h"
#include "../pawns/pawns.h"
#include "../timer/timer.h"
#include "../move/move.h"

struct GameInfo globalGameInfo;
struct Walls globalWalls;  
static void drawInitBoard();
static void drawInitTokens();
void initGame();
	
void initGame(){ //init global variables and draw board
	globalGameInfo.current_turn_player=0;
	globalWalls.index = 0;

	drawInitBoard();
	drawInitTokens();
}

static void drawInitTokens(){
	GUI_Text(113, 233, (uint8_t *) " ", White, Red);
	GUI_Text(113, 35, (uint8_t *) " ", White, Blue);
	initialPosition();
}

static void drawInitBoard(){
  int i = 0;
	int j = 0;
	int count = 0;
	int count2 = 0;
	int h = 0;
	int h2 = 0; 
	
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
}
