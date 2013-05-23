#include <avr/io.h>

void UART1_Init(unsigned int ubrr1)
{
	UBRR1H = (unsigned char)(ubrr1>>8);	// ������� ������� ������
	UBRR1L = (unsigned char) ubrr1;
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1); // ���������� �������, �������� �������, ���������� �� ������ ��������
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);	// ����������� �����, 8 ���, 1 ����-���, �������� �������� ����������
}

void UART1_SendByte(unsigned char byte1)
{
	while (!( UCSR1A & (1<<UDRE1)));
	UDR1 = byte1;
};

void UART1_SendString(char *buffer)
{
	while (*buffer!=0)
	{
		 UART1_SendByte(*buffer);
		 buffer++;
	}
}

/*
// ��������� ���������� �� ������ ����� �� UART (���������� � ������� ������)
ISR(USART1_RX_vect)
{
	unsigned char buff=UDR1;
}
*/
