#include <util/delay.h>
#include <avr/io.h>
#include "wh_lcd.h"

//������� ������ ������� 
void LcdWriteCom(unsigned char data)
{
  ClearBit(PORT_SIG, RS);	//��������� RS � 0 - �������
  PORT_DATA = data;		//����� ������ �� ���� ���������� 
  SetBit(PORT_SIG, EN);	        //��������� E � 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//��������� E � 0 - ������������ �����
  _delay_us(40);
}

//������� ������ ������
void LcdWriteData(unsigned char data)
{
  SetBit(PORT_SIG, RS);	        //��������� RS � 1 - ������
  PORT_DATA = data;		//����� ������ �� ���� ���������� 
  SetBit(PORT_SIG, EN);	        //��������� E � 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//��������� E � 0 - ������������ �����
  _delay_us(40);
}

void LcdWriteString(char string[8])
{
	unsigned char i;
	for (i=0; string[i]!='\0'; i++)
		{
		LcdWriteData(string[i]);
		}
}

void LcdSetCursor(char row, char col)
{
	switch (row)
	{
		case 0: LcdWriteCom(FIRST_STRING); break;
		case 1: LcdWriteCom(SECOND_STRING); break;
		default: return;
	} 
	LcdWriteCom(0x80|(1<<col));
}

//������� �������������
void InitLcd(void)
{
  DDRX_DATA = 0xff;
  PORT_DATA = 0xff;	
  DDRX_SIG = 0xff;
  PORT_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  ClearBit(PORT_SIG, RW);

  _delay_ms(40);
  LcdWriteCom(0x38); //0b00111000 - 8 ��������� ����, 2 ������
  LcdWriteCom(0xf);  //0b00001111 - �������, ������, �������� ��������
  LcdWriteCom(0x1);  //0b00000001 - ������� �������'
  _delay_ms(2);
  LcdWriteCom(0x6);  //0b00000110 - ������ �������� ������, ������ ���
}

