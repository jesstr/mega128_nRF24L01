#include "buttons.h"

/* Buttons initialization */
void Buttons_Init(void)
{
	/* Set all PINs as input with internal pull-ups  */ 
	BUTTON_1_DDR&=~(1<<BUTTON_1_PIN);
	BUTTON_1_PORT|=(1<<BUTTON_1_PIN);	
	
	BUTTON_2_DDR&=~(1<<BUTTON_2_PIN);
	BUTTON_2_PORT|=(1<<BUTTON_2_PIN);	
	
	SWITCH_1_DDR&=~(1<<SWITCH_1_PIN);
	SWITCH_1_PORT|=(1<<SWITCH_1_PIN);	
	
	SWITCH_2_DDR&=~(1<<SWITCH_2_PIN);
	SWITCH_2_PORT|=(1<<SWITCH_2_PIN);
}