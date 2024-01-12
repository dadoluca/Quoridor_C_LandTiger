#include <stdbool.h>
#include <stdio.h>
#include "../GLCD/GLCD.h"
#include "walls.h"
#include "../game/game.h"
#include "../movementLogic/movement_logic.h"

#define UPPERSPACE 29
extern struct Walls globalWalls;  

void draw_wall(int posX_start, int posY_start, int posX_end, int posY_end, int vertical, int player, bool cancel);
void draw_wall_wrapper(int move, bool cancel);
bool checkPosition(int move);
int getWallCentre(int x, int y, int vertical);
int getNextPlaceableWall();


void draw_wall_wrapper(int move, bool cancel){
	int x_end;
	int y_end;
	int vertical;
	int playerId;
	
	int x_start = move & 0xFF; 
	int y_start = move & 0xFF00;
	y_start = y_start >> 8;
	
	vertical= move & 0xF0000;
	vertical = vertical >> 16;
	
	if(vertical==0){
		x_end = x_start;
		y_end = y_start + 61;
	}
	else{
		x_end = x_start+ 61;
		y_end = y_start;
	}
	
	playerId = move & 0xF000000;
	playerId = playerId >> 24;
		
	draw_wall(x_start,y_start,x_end,y_end,vertical,playerId,cancel);
}

void draw_wall(int posX_start, int posY_start, int posX_end, int posY_end, int vertical, int player, bool cancel){
	int i=0;
	for(i=0;i<4;i++){//disegna il muro creando uno spessore di 4
		if(player==PLAYER_0)//green
		{
			if(cancel)
				LCD_DrawLine(posX_start, posY_start, posX_end, posY_end , White);
			else
				LCD_DrawLine(posX_start, posY_start, posX_end, posY_end , Green);
		}
		else{		//blue
			if(cancel)
				LCD_DrawLine(posX_start, posY_start, posX_end, posY_end , White);
			else
				LCD_DrawLine(posX_start, posY_start, posX_end, posY_end , Blue);
		}
		if(vertical==0){
			posX_start=posX_start+1;
			posX_end=posX_end+1;
		}else//horizontal
		{
			posY_start=posY_start+1;
			posY_end=posY_end+1;
		}
	}
}

int totalPlayerWalls(int playerId) {//funzione che serve per contare quanti muri ci sono per player id
	int count = 0;
	int user;
	int i;
	for (i = 0; i < globalWalls.index; i++) {
		user = globalWalls.wallsList[i] >> 24;
		if(user == playerId) { count++; }
	}
	return count;
}

bool finishedWalls(int playerId){
	if(totalPlayerWalls(playerId) >= MAX_WALLS_PER_USER){
		NVIC_DisableIRQ(EINT1_IRQn);
		return true;
	}
	//LPC_SC->EXTINT &= (1 << 1);
	NVIC_EnableIRQ(EINT1_IRQn);
	return false;
}

void writeWarningMessage(int playerId, bool cancel){
	if(cancel){
		GUI_Text(42, 3, (uint8_t *) "                       ", White, White);
		return;
	}
	//Pulisce da eventuali scritte e poi scrive
	GUI_Text(42, 3, (uint8_t *) "                       ", White, White);
	if(playerId == PLAYER_0){
		GUI_Text(42, 3, (uint8_t *) "Player 1 reached 8 walls", Black, Green);
	} else if(playerId == PLAYER_1){
		GUI_Text(42, 3, (uint8_t *) "Player 2 reached 8 walls", White, Blue);
	}
}


void updateCountWalls(int playerId){
	char str[1];
	int remainingWalls = MAX_WALLS_PER_USER;
	if(playerId == 0){
		remainingWalls = remainingWalls - totalPlayerWalls(0);
		sprintf(str, "%d", remainingWalls);
		GUI_Text(35, 298, (uint8_t *) " ", Black, White);
		GUI_Text(35, 298, (uint8_t *) str, Black, White);
	} else if(playerId == 1){
		remainingWalls = remainingWalls - totalPlayerWalls(1);
		sprintf(str, "%d", remainingWalls);
		GUI_Text(195, 298, (uint8_t *) " ", Black, White);		
		GUI_Text(195, 298, (uint8_t *) str, Black, White);
	}
}


int getWallCentre(int x, int y, int vertical){
	int res=0;
	if(vertical==0){
		x = x; 
		y = y + 28 + 5;
	}
	else{
		x = x+ 28 + 5; 
		y = y;
	}
	res |= x;
	res |= y<<8;
	return res;
}

bool checkPosition(int move){
	int vertical;
	int centre_coordinate;
	
	int x_start;
	int y_start;
	int x_centre;
	int y_centre;
	
	int x_start_current;
	int y_start_current;
	int x_centre_current;
	int y_centre_current;
	
	bool parzialmente_sovrapposti_1;
	bool parzialmente_sovrapposti_2;
	int distanza_x_centri;
	int distanza_y_centri;	
	bool centri_sovrapposti;
	
	bool out_up_limit = false;
	bool out_down_limit = false;
	bool out_left_limit = false;
	bool out_right_limit = false;
	
	int i;
	int is_placeable = true;


	x_start = move & 0xFF; 
	y_start = move & 0xFF00;
	y_start = y_start >> 8;
	
	vertical= move & 0xF0000;
	vertical = vertical >> 16;
	
	centre_coordinate = getWallCentre(x_start,y_start,vertical);
	x_centre = centre_coordinate & 0xFF; 
	y_centre = centre_coordinate & 0xFF00;
	y_centre = y_centre >> 8;
	
	
	
	if(y_start > 250){
		return false;
	}
	
	out_up_limit = vertical==0 ? y_start < UPPERSPACE : y_start < UPPERSPACE + 28; 
	out_down_limit =  vertical==0 ?  y_start > UPPERSPACE + 226 -(28+5+28): y_start > UPPERSPACE + 226;
	out_left_limit = x_start < 7-4;//-4 perché non ben centrata
	out_right_limit = vertical==0 ?  x_start > 226 : x_start > 226+7-(28+5+28);//7 di margine bianco prima e dopo la scacchiera
	//controllo che non esca dalla scacchiera
	if(out_up_limit || out_down_limit || out_left_limit || out_right_limit)
		return false;
	
	//scorriamo walls list
	for(i=0;i<globalWalls.index;i++){
		x_start_current = globalWalls.wallsList[i] & 0xFF;
		y_start_current = globalWalls.wallsList[i] & 0xFF00;
		y_start_current = y_start_current >> 8;
		
		vertical= globalWalls.wallsList[i] & 0xF0000;
	  vertical = vertical >> 16;
		centre_coordinate = getWallCentre(x_start_current,y_start_current,vertical);
		x_centre_current = centre_coordinate & 0xFF;
		y_centre_current = centre_coordinate & 0xFF00;
	  y_centre_current = y_centre_current >> 8;
		//ora abbiamo tutte le posizioni iniziali e centrali di entrambi
		
		//bool completamente_sovrapposti = (x_start_current==x_start && y_start_current==y_start);
		parzialmente_sovrapposti_1= (x_start_current==x_centre && y_start_current== y_centre);
		parzialmente_sovrapposti_2= (x_centre_current==x_start && y_centre_current== y_start);
		distanza_x_centri = x_centre_current - x_centre;
		distanza_y_centri = y_centre_current - y_centre;
		//centri sovrapposti funziona anche per completamente sovrapposti
		centri_sovrapposti = (distanza_x_centri > -5 && distanza_x_centri < 5 && distanza_y_centri > -5 && distanza_y_centri< 5);
		if(parzialmente_sovrapposti_1 || parzialmente_sovrapposti_2 || centri_sovrapposti){
			is_placeable = false;
		}
	}
	
	return is_placeable;
	
}

int getNextPlaceableWall(){
	int xW;
	int yW;
	bool up = true;
	
	int current_default_beginning_wall = globalGameInfo.current_turn_player==1 ? 0x1119D68 : 0x0119D68;	
	
	while(!checkPosition(current_default_beginning_wall)){
		xW = current_default_beginning_wall & 0xFF;
		yW = current_default_beginning_wall & 0xFF00;
		yW = yW >> 8;
		if(UPPERSPACE > yW-33 || !up){
			updateMoveDown(&current_default_beginning_wall);
			up = false;
		} else if(up/*yW + 33 > 233*/){
			updateMoveUp(&current_default_beginning_wall);
		}
	}
	
	return current_default_beginning_wall;
}


