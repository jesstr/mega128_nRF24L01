//требуется библиотека spi.c, spi.h
#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "nRF24L01.h"

void nRF24L01_Init(void)
{
	CE_DDR|=(1<<CE_PIN);
	CE_PORT&=~(1<<CE_PIN);
	IRQ_DDR&=~(1<<IRQ_PIN);		
}

void nRF24L01_SendData(unsigned char *data, unsigned char length)
{
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_TX_PAYLOAD);
	while (length)
	{
		SPI_SendByte_Master(*data);
		data++;
		length--;		
	}
	SPI_CS1_HIGH;
	CE_PULSE;	
}


void nRF24L01_SetRXPayloadLenght(unsigned char length)
{
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|RX_PW_P0);
	SPI_SendByte_Master(length & 0x1F);
	SPI_CS1_HIGH;	
}


void nRF24L01_Receive_On(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff|_BV(PRIM_RX));
	SPI_CS1_HIGH;
	CE_HIGH;
	_delay_us(2);		
}


void nRF24L01_Receive_Off(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff&(~_BV(PRIM_RX)));
	SPI_CS1_HIGH;
	CE_LOW;
	_delay_us(2);		
}


void nRF24L01_ReadData(unsigned char *data)
{
	SPI_CS1_LOW;
	
	SPI_CS1_HIGH;		
}

void nRF24L01_Standby_1(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff|_BV(PWR_UP));
	SPI_CS1_HIGH;
}

void nRF24L01_PowerDown(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff&(~_BV(PWR_UP)));
	SPI_CS1_HIGH;		
}

unsigned char nRF24L01_GetStatus(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	return buff;		
}

unsigned char nRF24L01_GetSConfig(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	return buff;	 
}