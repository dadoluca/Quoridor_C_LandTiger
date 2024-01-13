/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../tokens/token.h"
#include "../walls/walls.h"
#include "../RIT/RIT.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int quartersOfSecond = 4; 
int sec = 20;
bool reset = false;

extern int m;

void updateSec(uint8_t* sec) {
	GUI_Text(105, 287, sec, Black, White);
}

void TIMER0_IRQHandler (void)
{
	if(reset) {
		sec = 20;
		quartersOfSecond=4;
		reset = false;
	}	else {
		char str[2];
		if(sec > 9) {
			sprintf(str, "%d", sec);
		} else {
			sprintf(str, "0%d", sec);
		}
		updateSec(str);
		quartersOfSecond--;
		if(quartersOfSecond<=0){
			sec--;
			quartersOfSecond=4;
		}
		sec = quartersOfSecond<=0? sec--: sec;
		if(sec<=0){
			confirmEndTurn();
			m = 0;
		}
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	writeWarningMessage(0x0, true);
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
	disable_timer(1);
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
