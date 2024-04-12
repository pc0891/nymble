/*
 * serial.c
 *
 * Created: 11/04/2024 14:11:10
 *  Author: pramod
 */ 
#include "serial.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile char bufferRx[100], bufferTx[100];
volatile unsigned char count, endOfRx , endOfBlockRx, sendACK, endOfBlockTx, getACK ,ch;

//intialize UART interrupt
void uartInit()
{
	UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	UBRR0L = 0x9F;
	UBRR0H = 1;
}

//receive interrupt routine
ISR (USART_RX_vect)
{
	if(getACK!=1)
	{
		bufferRx[count]=UDR0;
		switch(bufferRx[count])
		{
			case 3:
				endOfRx=1;
				endOfBlockRx=1;
				break;
			case 4:
				endOfBlockRx=1;
				break;
			default:
				count++;
		}	
	}
	else
	{
		ch=UDR0;
		if(ch==17)
		{
			getACK=0;
			endOfBlockTx=0;
		}
	}
}

//trasmit interrupt routine
ISR (USART_TX_vect)
{
	if(sendACK!=1)
	{
		switch(bufferTx[count])
		{
			case 3:
			case 4:
				count=0;
				endOfBlockTx=1;
				break;
			default:
				count++;
				UDR0=bufferTx[count];
		}
	}
	else
	{
		endOfBlockRx=0;
		count=0;
		sendACK=0;
	}
}