#include <util/delay.h>
#include <avr/io.h>
#include "wh_lcd.h"

//функция записи команды 
void LcdWriteCom(unsigned char data)
{
  ClearBit(PORT_SIG, RS);	//установка RS в 0 - команды
  PORT_DATA = data;		//вывод данных на шину индикатора 
  SetBit(PORT_SIG, EN);	        //установка E в 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//установка E в 0 - записывающий фронт
  _delay_us(40);
}

//функция записи данных
void LcdWriteData(unsigned char data)
{
  SetBit(PORT_SIG, RS);	        //установка RS в 1 - данные
  PORT_DATA = data;		//вывод данных на шину индикатора 
  SetBit(PORT_SIG, EN);	        //установка E в 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//установка E в 0 - записывающий фронт
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

//функция инициализации
void InitLcd(void)
{
  DDRX_DATA = 0xff;
  PORT_DATA = 0xff;	
  DDRX_SIG = 0xff;
  PORT_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  ClearBit(PORT_SIG, RW);

  _delay_ms(40);
  LcdWriteCom(0x38); //0b00111000 - 8 разрядная шина, 2 строки
  LcdWriteCom(0xf);  //0b00001111 - дисплей, курсор, мерцание включены
  LcdWriteCom(0x1);  //0b00000001 - очистка дисплея'
  _delay_ms(2);
  LcdWriteCom(0x6);  //0b00000110 - курсор движется вправо, сдвига нет
}

