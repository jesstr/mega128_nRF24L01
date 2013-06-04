#ifndef _BUTTONS_H_
#define _BUTTONS_H_ 1

/* Buttons registers defines */
#define BUTTON_1_DDR		DDRA
#define BUTTON_1_PORT		PORTA
#define BUTTON_1_PIN_REG	PINA
#define BUTTON_1_PIN		PA0

#define BUTTON_2_DDR		DDRA
#define BUTTON_2_PORT		PORTA
#define BUTTON_2_PIN_REG	PINA
#define BUTTON_2_PIN		PA1

/* Switch registers defines */
#define SWITCH_1_DDR		DDRA
#define SWITCH_1_PORT		PORTA
#define SWITCH_1_PIN_REG	PINA
#define SWITCH_1_PIN		PA2

#define SWITCH_2_DDR		DDRD
#define SWITCH_2_PORT		PORTD
#define SWITCH_2_PIN_REG	PIND
#define SWITCH_2_PIN		PD5

/* Button names defines */
#define MODE_BUTTON		(!(BUTTON_1_PIN_REG & (1<<BUTTON_1_PIN)))
#define FLASH_BUTTON	(!(BUTTON_2_PIN_REG & (1<<BUTTON_2_PIN)))
#define ON_OFF_SWITCH	(!(SWITCH_1_PIN_REG & (1<<SWITCH_1_PIN)))
#define SOUND_SWITCH	(!(SWITCH_2_PIN_REG & (1<<SWITCH_2_PIN)))

#endif /* _BUTTONS_H_ */