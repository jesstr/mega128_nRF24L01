/*
 * mega128_nRF24L01.c
 *
 * Created: 17.07.2012 11:26:44
 *  Author: Pavel Cherstvov
 */

/*FCU must be defined at "Properties->Toolchain->Symbols" as "F_CPU=8000000". */ 
/* #define F_CPU 8000000 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"
#include "spi.h"
#include "nRF24L01.h"
#include "wh_lcd.h"
#include "commands.h"
#include "buttons.h"

/* Overall number of light modes */
#define	LIGHT_MODES_NUMBER	3 
/* Default light mode: 1 - COMM_INSTANT */
#define DEFAULT_LIGHT_MODE  1

#define RXPayloadLenght 3					/* Fixed RX data packet length in bytes */ 
#define TXPayloadLenght RXPayloadLenght		/* Fixed TX data packet length in bytes */ 

unsigned char rx_payload[RXPayloadLenght+1];
unsigned char tx_payload[TXPayloadLenght+1]={0x01,0x02,0x03,0x00};

/* Start Timer0 with divider 1024 */
#define TIMER0_DIVIDER		1024
#define START_TIMER0		TCCR0|=(1<<CS02)|(1<<CS01)|(1<<CS00)
#define STOP_TIMER0			TCCR0&=~(1<<CS02)|(1<<CS01)|(1<<CS00)

/* Status leds registers defines */
#define ERROR_LED_DDR	DDRD
#define ERROR_LED_PORT	PORTD
#define ERROR_LED_PIN	PD6

#define MODE_LED_DDR	DDRD
#define MODE_LED_PORT	PORTD
#define MODE_LED_PIN	PD7

/* Flashes per second defines */
#define FASTFLASH_LED_FPS	15
#define SLOWFLASH_LED_FPS	5
#define ERROR_LED_FPS		3

/* Counter of Timer ticks for mode led */
unsigned char mode_led_counter=0;
/* Flashes per second for mode led */
volatile unsigned char mode_led_fps=0;
/* Counter of Timer ticks for error led */
unsigned char error_led_counter=0;
/* Flashes per second for error led */
volatile unsigned char error_led_fps=0;

volatile unsigned char abv=0;

/* Leds control commands */
#define MODE_LED_START_FAST		do{\
								mode_led_fps=FASTFLASH_LED_FPS;\
								}while(0)							
#define MODE_LED_START_SLOW		do{\
								mode_led_fps=SLOWFLASH_LED_FPS;\
								}while(0)
#define MODE_LED_START_INSTANT	do{\
								mode_led_fps=0;\
								MODE_LED_PORT|=(1<<MODE_LED_PIN);\
								}while(0)								
#define MODE_LED_STOP			do{\
								mode_led_fps=0;\
								MODE_LED_PORT&=~(1<<MODE_LED_PIN);\
								}while(0)			
#define ERROR_LED_START			do{\
								error_led_fps=ERROR_LED_FPS;\
								}while(0)
#define ERROR_LED_STOP			do{\
								error_led_fps=0;\
								ERROR_LED_PORT&=~(1<<ERROR_LED_PIN);\
								}while(0)	

/* Delay defines */
#define SWITCH_DELAY	_delay_ms(100)
#define BUTTON_DELAY	_delay_ms(500)

/* Marker variables */
volatile unsigned char pwr_on; 		/* pwr_on = 1 - device is switched on
									   pwr_on = 0 - device is switched off */

volatile unsigned char lightmode=0;	/* Light mode */

volatile unsigned char tx_error=0; 	/* tx_error = 1 - TX error has occurred
									   tx_error = 0 - TX was successful */

volatile unsigned char re=0;


char *IntToStr(int value, int radix)
{
	char buffer[8];
	char *ptr;
	ptr = itoa(value, buffer, radix);
	return ptr;
}


/* Interrupts first initialization */
void IRQ_Init(void)
{
	EIMSK|=(1<<INT0);
}


/* Status leds initialization */
void Leds_Init(void)
{
	ERROR_LED_DDR|=(1<<ERROR_LED_PIN);
	ERROR_LED_PORT&=~(1<<ERROR_LED_PIN);
	
	MODE_LED_DDR|=(1<<MODE_LED_PIN);
	MODE_LED_PORT&=~(1<<MODE_LED_PIN);
}

/* Timer0 initialization (Timer stopped) */
void Timer0_Init(void) 
{
	TIMSK|=(1<<TOIE0);
}


/* Timer0 overflow interrupt handle */
ISR(TIMER0_OVF_vect) 
{
	mode_led_counter++;
	error_led_counter++;
	if ((mode_led_fps > 0) && (mode_led_counter >= F_CPU/TIMER0_DIVIDER/256/mode_led_fps/2)) {
		mode_led_counter=0;
		MODE_LED_PORT^=(1<<MODE_LED_PIN);
	}
	if ((error_led_fps > 0) && (error_led_counter >= F_CPU/TIMER0_DIVIDER/256/error_led_fps/2)) {
		error_led_counter=0;
		ERROR_LED_PORT^=(1<<ERROR_LED_PIN);
	}
	#if 0
	if (ERROR_LED_PORT & (1<<ERROR_LED_PIN)) 
		ERROR_LED_PORT&=~(1<<ERROR_LED_PIN);
	else
		ERROR_LED_PORT|=(1<<ERROR_LED_PIN);
	#endif
}


/* UART RX interrupt handle */
ISR(USART1_RX_vect)
{
	unsigned char buff=UDR1;
	if (tx_payload[0]==0x01) tx_payload[0]=0x02;
		else tx_payload[0]=0x01;
}


/* INT0 interrupt handle, connected to the nRF24L01 "IRQ" pin */
ISR(INT0_vect)
{
	unsigned char buff, i=0;
	
	/* Reading the status register */
	SPI_CS1_LOW;
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	
	/* UART debug output */
 	UART1_SendString("Status: ");
 	UART1_SendString(IntToStr(buff,16));
 	UART1_SendString("\x0A\x0D");
	
	/* Checking interrupt flags in status register */
	switch ((buff)&(0x70)) {
		/* RX_DR - RX data ready */
		case 0x40: {
			/* UART debug output */
			UART1_SendString("RX_DR"); 
			
			CE_LOW;
			
			/* Reading RX buffer */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(R_RX_PAYLOAD); 
			while (i<RXPayloadLenght) { rx_payload[i++]=SPI_ReceiveByte_Master(); }
			SPI_CS1_HIGH;
			
			rx_payload[RXPayloadLenght]=0x00; 
			
			/* UART debug output */
			UART1_SendString(IntToStr(rx_payload[0],16)); 
			
			_delay_us(2);
			
			/* Clearing IRQ flag */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x40)); 
			SPI_CS1_HIGH; 
			
			/* UART debug output */
			UART1_SendString(IntToStr(nRF24L01_GetStatus(),16));
			
			CE_HIGH;

			break;  
		}	
		/* TX_DS - data sent */	
		case 0x20: {
			/* UART debug output */
			UART1_SendString("TX_DS"); 
			
			/* Clearing IRQ flag */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x20));
			SPI_CS1_HIGH; 
			
			ERROR_LED_STOP;
			switch (lightmode){
				case 0: MODE_LED_STOP; break;
				case 1:	MODE_LED_START_INSTANT; break;
				case 2:	MODE_LED_START_FAST; break;
				case 3:	MODE_LED_START_SLOW; break;
				default: break;
			}
			
			/* LCD debug output */
			LcdWriteCom(SECOND_STRING);
			LcdWriteString("OK  ");
			
			break;  
		}		
		/* Error: MAX count of TX attempts reached */
		case 0x10: {
			/* UART debug output */
			UART1_SendString("MAX_RT"); 
			
			/* Clearing IRQ flag */		
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x10)); 
			SPI_CS1_HIGH; 
			
			tx_error=1;
			MODE_LED_STOP;
			ERROR_LED_START;
			
			/* LCD debug output */
			LcdWriteCom(SECOND_STRING);
			LcdWriteString("Fail");
			
			break;  
		}		
	}		
	/* UART debug output */
	UART1_SendString("\x0A\x0D");
}

/* Send a wireless command function */
void SendCommand(unsigned char command) {
	tx_payload[0]=command;
	nRF24L01_SendData(tx_payload, RXPayloadLenght);
}


/* Main routine */
int main(void)
{
	unsigned char count=0; 
	
	IRQ_Init();	
	InitLcd();
	nRF24L01_Init();
	UART1_Init(MYUBRR);
	SPI_Init_Master();
	Buttons_Init();
	Leds_Init();
	Timer0_Init();
	START_TIMER0;
	
	MODE_LED_PORT|=(1<<MODE_LED_PIN);
	ERROR_LED_PORT|=(1<<ERROR_LED_PIN);
	
	nRF24L01_Standby_1();
	nRF24L01_SetRXPayloadLenght(RXPayloadLenght);
	//nRF24L01_Receive_On();
	sei();
	
	/* LCD debug output */
	LcdWriteCom(SECOND_STRING);
	LcdWriteString("Ready");
	
	/* UART debug output */
 	UART1_SendString(IntToStr(nRF24L01_GetSConfig(),16));
 	UART1_SendByte(0x0A);
	
	/* Main loop */
    while(1)
    {
		/* Buttons and switches polling */
		if (MODE_BUTTON && (lightmode != 0)) {
			/* Checking light mode */
			if (lightmode != LIGHT_MODES_NUMBER) 
				lightmode++;
			else 
				lightmode=1;
			switch (lightmode) {
				case 1: SendCommand(COMM_INSTANT); break;
				case 2: SendCommand(COMM_FASTFLASH); break;
				case 3: SendCommand(COMM_SLOWFLASH); break;
				default: break;
			}
			BUTTON_DELAY;
		}
		else if (FLASH_BUTTON) {
			/* Flash mode of front light while button is pressed  */
		}
		else if ((ON_OFF_SWITCH == 1)  && (lightmode == 0)) {
			lightmode=DEFAULT_LIGHT_MODE;
			SendCommand(COMM_INSTANT);
			SWITCH_DELAY;
		}
		else if ((ON_OFF_SWITCH == 0) && (lightmode != 0)) {
			lightmode=0;
			SendCommand(COMM_OFF);
			SWITCH_DELAY;
		}
		else if (SOUND_SWITCH == 1) {
			/* Beep sound on */
		}
		else if (SOUND_SWITCH == 0) {
			/* Beep sound off */
		}
		//_delay_ms(250);
	}
}
