/*
 * mega128_nRF24L01.c
 *
 * Created: 17.07.2012 11:26:44
 *  Author: Черствов
 */

/*FCU must be defined at "Properties->Toolchain->Symbols" as "F_CPU=16000000". */ 
/* #define F_CPU 16000000 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"
#include "spi.h"
#include "nRF24L01.h"
#include "wh_lcd.h"
#include "commands.h"

#define RXPayloadLenght 3					/* Fixed RX data packet length in bytes */ 
#define TXPayloadLenght RXPayloadLenght		/* Fixed TX data packet length in bytes */ 

unsigned char rx_payload[RXPayloadLenght+1];
unsigned char tx_payload[TXPayloadLenght+1]={0x01,0x02,0x03,0x00};

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
			
			/* LCD debug output */
			LcdWriteCom(SECOND_STRING);
			LcdWriteString("Fail");
			
			break;  
		}		
	}		
	/* UART debug output */
	UART1_SendString("\x0A\x0D");
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
	
	nRF24L01_Standby_1();
	nRF24L01_SetRXPayloadLenght(RXPayloadLenght);
	//nRF24L01_Receive_On();
	sei();
	
	/* UART debug output */
 	UART1_SendString(IntToStr(nRF24L01_GetSConfig(),16));
 	UART1_SendByte(0x0A);
	
	/* Main loop */
    while(1)
    {
		/* test */
		tx_payload[0]=COMM_ON;
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		_delay_ms(1000);
		tx_payload[0]=COMM_SLOWFLASH;
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		_delay_ms(1000);
		tx_payload[0]=COMM_FASTFLASH;
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		_delay_ms(1000);
		tx_payload[0]=COMM_INSTANT;
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		_delay_ms(1000);
		tx_payload[0]=COMM_OFF;
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		_delay_ms(2000);
		
		#if 0
		/* LCD debug output */
		LcdWriteCom(FIRST_STRING);
		LcdWriteString("        ");
		LcdWriteCom(FIRST_STRING);
		LcdWriteString("Sent ");
		LcdWriteString(IntToStr(count+1,10));	
		
		/* UART debug output */
		UART1_SendString(IntToStr(count+1,10));
		UART1_SendString("\x0A\x0D");
		
		count++;
		_delay_ms(1000);
		#endif    
	}
}
