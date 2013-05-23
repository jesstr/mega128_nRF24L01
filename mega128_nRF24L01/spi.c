#include <avr/io.h>
#include "spi.h"
   
   // ������������� ����������� SPI � ������ Master
  void SPI_Init_Master(void)
  {
	SPI_DDR |= _BV(CS1) | _BV(SCK) | _BV(MOSI);	
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA);	/* Enable SPI mode 1, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);				/* Enable SPI mode 0, Master, set clock rate fck/16 */
	SPI_CS_PORT|= _BV(CS1);
  }
   
   // ������������� ����������� SPI � ������ Slave
  void SPI_Init_Slave(void)
  {
	SPI_DDR = (1<<MISO);
	SPCR = (1<<SPE)|(1<<CPHA); 	/* Enable SPI, Slave */
  }
   
  // �������� ����� �� SPI � ������ Master
  void SPI_SendByte_Master(unsigned char data)
  {
		SPDR = data;	//������
		while(!(SPSR & (1<<SPIF)))
			; 
  }
  
  // �������� ����� �� SPI � ������ Slave
  void SPI_SendByte_Slave(unsigned char data)
  {
		unsigned char temp;
		SPDR = data;	//������
		while(!(SPSR & (1<<SPIF)))
			;
		temp=SPDR;
  }
  
  // ����� ����� �� SPI � ������ Master
  unsigned char SPI_ReceiveByte_Master(void)
  {
		SPDR = 0xff;	//������������ ������
		while(!(SPSR & (1<<SPIF)))
			; 
		return SPDR;
  }  
  
  // ����� ����� �� SPI � ������ Slave
  unsigned char SPI_ReceiveByte_Slave(void)
  {
		while(!(SPSR & (1<<SPIF)))
			; 
		return SPDR;
  }  