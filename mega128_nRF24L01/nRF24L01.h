
#ifndef _NRF24L01_H_
#define _NRF24L01_H_ 1

#define CE_PIN PB4
#define CE_PORT PORTB
#define CE_DDR DDRB

#define IRQ_PIN PD0
#define IRQ_PORT PORTD
#define IRQ_DDR DDRD

#define CE_LOW	CE_PORT&=~_BV(CE_PIN);
#define CE_HIGH CE_PORT|=_BV(CE_PIN);

#define CE_PULSE	CE_HIGH; \
					_delay_us(15); \
					CE_LOW;

//������ ������
#define R_REGISTER				0b00000000 // + 5 ������� ��� ������ ��������
#define W_REGISTER				0b00100000 // + 5 ������� ��� ������ ��������
#define R_RX_PAYLOAD			0b01100001
#define W_TX_PAYLOAD			0b10100000
#define FLUSH_TX				0b11100001
#define FLUSH_RX				0b01100010
#define REUSE_TX_PL				0b11100011
#define ACTIVATE				0b01010000
#define R_RX_PL_WID				0b01100000
#define W_ACK_PAYLOAD			0b10101000 // + 3 ������� ���� payload (000 - 101)
#define W_TX_PAYLOAD_NO_ACK		0b10110000
#define NOP						0b11111111

//������ ���������
#define CONFIG	0x00
#define STATUS	0x07
#define RX_PW_P0	0x11

//����������� ��������� �����
#define PWR_UP 1
#define PRIM_RX 0

// #define nRF24L01_Standby_1		SPI_CS1_LOW;
// 								SPI_SendByte_Master(R_REGISTER|CONFIG);
// 								buff=SPI_ReceiveByte_Master();
// 								SPI_CS1_HIGH;
	
void nRF24L01_Init(void);
void nRF24L01_SendData(unsigned char *data, unsigned char length);
void nRF24L01_SetRXPayloadLenght(unsigned char length);
void nRF24L01_Receive_On(void);
void nRF24L01_Receive_Off(void);
void nRF24L01_ReadData(unsigned char *data);
void nRF24L01_Standby_1(void);
void nRF24L01_PowerDown(void);
unsigned char nRF24L01_GetStatus(void);
unsigned char nRF24L01_GetSConfig(void);

#endif /* _NRF24L01_H_ */
