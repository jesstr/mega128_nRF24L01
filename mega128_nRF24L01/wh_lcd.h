
#ifndef _WH_LCD_H_
#define _WH_LCD_H_ 1

//���� � �������� ���������� ���� ������ ���
#define PORT_DATA PORTC
#define PIN_DATA  PINC
#define DDRX_DATA DDRC

//���� � �������� ���������� ����������� ������ ���
#define PORT_SIG PORTA
#define PIN_SIG  PINA
#define DDRX_SIG DDRA

//������ ������� � ������� ���������� ����������� ������ ��� 
#define RS 3
#define RW 5
#define EN 4

//������� ��� ������ � ������
#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit))	

#define SHIFT_DISP_RIGHT 0x1C
#define SHIFT_DISP_LEFT 0x18
#define SHIFT_CURR_RIGHT 0x14
#define SHIFT_CURR_LEFT 0x10
#define SECOND_STRING 0xC0
#define FIRST_STRING 0x80

//������� ������ ������� 
void LcdWriteCom(unsigned char data);
//������� ������ ������
void LcdWriteData(unsigned char data);
void LcdWriteString(char string[8]);
void LcdSetCursor(char row, char col);
void InitLcd(void);

#endif /* _WH_LCD_H_ */


