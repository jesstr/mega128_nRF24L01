/*
 * mega128_nRF24L01.c
 *
 * Created: 17.07.2012 11:26:44
 *  Author: Черствов
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "spi.h"
#include "nRF24L01.h"
#include "wh_lcd.h"

#define RXPayloadLenght 3			/* Fixed RX data packet length in bytes */ 
#define TXPayloadLenght RXPayloadLenght		/* Fixed TX data packet length in bytes */ 

unsigned char rx_payload[RXPayloadLenght];
unsigned char tx_payload[TXPayloadLenght]={0x31,0x32,0x33};

volatile unsigned char re=0;

char *IntToStr(int value, int radix)
{
char buffer[8];
char *ptr;
ptr = itoa(value, buffer, radix);
return ptr;
}


ISR(USART1_RX_vect)
{
	unsigned char buff=UDR1;
}


ISR(INT0_vect)
{
	unsigned char buff, i=0;
	
	SPI_CS1_LOW;
	buff=SPI_ReceiveByte_Master(); //чтение статусного регистра
	SPI_CS1_HIGH;
// 	UART1_SendString("Status: ");
// 	UART1_SendString(IntToStr(buff,16));
// 	UART1_SendString("\x0A\x0D");
	
	switch ((buff)&(0x70)) // Проверка флагов прерываний в статусном регистре
	{
		case 0x40: // RX_DR - приход данных
		{
			UART1_SendString("RX_DR"); 
			
			CE_LOW;
			
			//чтение буфера приема
			SPI_CS1_LOW; 
			SPI_SendByte_Master(R_RX_PAYLOAD); 
			while (i<RXPayloadLenght) { rx_payload[i++]=SPI_ReceiveByte_Master(); }
			SPI_CS1_HIGH;
			
//			rx_payload[RXPayloadLenght]=0x00; 
			
// 			UART1_SendByte(IntToStr(nRF24L01_GetStatus(rx_payload[0]),16));
// 			UART1_SendByte(IntToStr(nRF24L01_GetStatus(rx_payload[1]),16));
// 			UART1_SendByte(IntToStr(nRF24L01_GetStatus(rx_payload[2]),16));
			
//			UART1_SendString(rx_payload); 
			
			_delay_us(2);
			
			//сброс флага прерывания
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x40)); 
			SPI_CS1_HIGH; 
			
			
//			UART1_SendString(IntToStr(nRF24L01_GetStatus(),16));
			
			CE_HIGH;

			
			break;  
		}		
		case 0x20: // TX_DS - данные отправлены
		{
			UART1_SendString("TX_DS"); 
			
			//сброс флага прерывания
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x20));
			SPI_CS1_HIGH; 
			
// 			LcdWriteCom(SECOND_STRING);
// 			LcdWriteString("   ");
			LcdWriteCom(SECOND_STRING);
			LcdWriteString("OK  ");
			
			break;  
		}		
		case 0x10: // MAX_RT - Достигнут лимит повторов отправки
		{
			UART1_SendString("MAX_RT"); 
			
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x10)); 
			SPI_CS1_HIGH; 
			
// 			LcdWriteCom(SECOND_STRING);
// 			LcdWriteString("   ");
			LcdWriteCom(SECOND_STRING);
			LcdWriteString("Fail");
			
			break;  
		}		
	}		
	UART1_SendString("\x0A\x0D");
	
	
}


int main(void)
{
	unsigned char count; 
	
	EIMSK=0x01;
	
	InitLcd();
	nRF24L01_Init();
	UART1_Init(MYUBRR);
	SPI_Init_Master();
	
 	LcdWriteString("HELLO");
 	UART1_SendString("HELLO");
	
	nRF24L01_Standby_1();
	nRF24L01_SetRXPayloadLenght(RXPayloadLenght);
//	nRF24L01_Receive_On();
	sei();
	
// 	UART1_SendString(IntToStr(nRF24L01_GetSConfig(),16));
// 	UART1_SendByte(0x0A);
	
	
    while(1)
    {
		
		nRF24L01_SendData(tx_payload, RXPayloadLenght);
		
		LcdWriteCom(FIRST_STRING);
		LcdWriteString("        ");
		LcdWriteCom(FIRST_STRING);
		LcdWriteString("Sent ");
		LcdWriteString(IntToStr(count+1,10));	
		UART1_SendString(IntToStr(count+1,10));
		UART1_SendString("\x0A\x0D");
		count++;
		_delay_ms(1000);
			
// 	if (re!=0) 
// 		{
// 			UART1_SendString("retransmit1\x0A");
// 			nRF24L01_Receive_Off();
// 			nRF24L01_SendData(rx_payload, RXPayloadLenght);
// 			
// 			LcdWriteCom(SECOND_STRING);
// 			LcdWriteString("   ");
// 			LcdWriteCom(SECOND_STRING);
// 			LcdWriteString(rx_payload);
// 			UART1_SendString("retransmit2\x0A");
// 			re=0;
// 		}
    }
}
