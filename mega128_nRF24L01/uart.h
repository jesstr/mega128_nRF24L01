
#ifndef _UART_H_
#define _UART_H_ 1

/*FCU must be defined at "Properties->Toolchain->Symbols" as "F_CPU=16000000". */
//#define F_CPU 16000000

#define BAUDRATE 9600
#define MYUBRR (F_CPU/16/BAUDRATE-1)

void UART1_Init(unsigned int ubrr1);
void UART1_SendByte(unsigned char byte1);
void UART1_SendString(char *buffer);

#endif /* _UART_H_ */
