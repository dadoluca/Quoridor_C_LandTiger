#include <stdio.h>
#include "game.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../walls/walls.h"
#include "../tokens/token.h"
#include "../timer/timer.h"
#include "../movementLogic/movement_logic.h"

struct GameInfo globalGameInfo;
struct Walls globalWalls;  
static void drawinitializeGameBoard();
static void drawInitTokens();
void initGame();
	
void initGame(){ //init global variables and draw board
	globalGameInfo.current_turn_player=0;
	globalWalls.index = 0;
	globalGameInfo.current_move_mode = TOKEN_MODE;
	globalGameInfo.walls_placed_p0=0;
	globalGameInfo.walls_placed_p1=0;
	
	drawinitializeGameBoard();
	drawInitTokens();
}

static void drawInitTokens(){
	int i;
	//---------------------------------------- draws PLAYER_0
	int posX=113;
	int posY=233;
	for(i=0;i<17;i++){	
		LCD_DrawLine(posX, posY, posX+10, posY , Green);
		if(i>=4 && i<=6){
			LCD_DrawLine(posX+2, posY, posX+4, posY , Black);
			LCD_DrawLine(posX+6, posY, posX+8, posY , Black);
		}else if(i>=11 && i<=13){
			LCD_DrawLine(posX+2, posY, posX+8, posY , Red);
		}
		posY++;
	}
	
	//---------------------------------------- draws PLAYER_1
	posY=35;
	for(i=0;i<17;i++){	
		LCD_DrawLine(posX, posY, posX+10, posY , Blue);
		if(i>=4 && i<=6){
			LCD_DrawLine(posX+2, posY, posX+4, posY , White);
			LCD_DrawLine(posX+6, posY, posX+8, posY , White);
		}else if(i>=11 && i<=13){
			LCD_DrawLine(posX+2, posY, posX+8, posY , Red);
		}
		posY++;
	}

	initialPosition();
}

static void drawinitializeGameBoard(){
  int i = 0;
	int j = 0;
	int count = 0;
	int count2 = 0;
	int h = 0;
	int h2 = 0; 
	
	LCD_Clear(White);
	
	//------------------------------------------------ chessboard informations boxes
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
	
	//------------------------------------------------ cessboard squares
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
