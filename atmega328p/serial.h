/*
 * serial.h
 *
 * Created: 11/04/2024 14:10:56
 *  Author: pramo
 */ 
#ifndef __SERIAL_H_
#define __SERIAL_H_

#define F_CPU 16000000UL
#define USART_BAUDRATE 2400
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

extern volatile unsigned char count, endOfRx, endOfBlockRx, sendACK, endOfBlockTx, getACK;
extern volatile char bufferRx[100], bufferTx[100];

void uartInit();
ISR (USART_RX_vect);
ISR (USART_TX_vect);

#endif
