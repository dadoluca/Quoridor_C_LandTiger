#include "button.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../timer/timer.h"

extern int down_key1;
extern int down_key2;
extern int down_eint0;


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



