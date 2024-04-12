/*
 * serialInt.c
 *
 * Created: 11/04/2024 10:55:04
 * Author : pramo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "serial.h"

void sendBlock(int length, int* textCount, char endOfText)
{
	eeprom_busy_wait();
	eeprom_read_block((void*)bufferTx,(const void*)*textCount,length);
	eeprom_busy_wait();
	if(endOfText==1)
		bufferTx[length]=3;
	else
		bufferTx[length]=4;
	count=0;
	getACK=1;
	UDR0=bufferTx[0];
	while(endOfBlockTx==0);
	(*textCount)+=length;
}


int main(void)
{
    int textLength,sentTextCount, remainTextCount;
	
	uartInit();
	sei();
	
	
	while(1)
	{
		textLength=0;
		endOfRx=0;
		endOfBlockRx=0;
		count=0;
		
		while(endOfRx==0)
		{
			while(endOfBlockRx==0);
			eeprom_busy_wait();
			eeprom_write_block((const void*)bufferRx,(void*)textLength,count);
			textLength+=count;
			UDR0=17;
			sendACK=1;
			while(endOfBlockRx==1);
		}
		
		sentTextCount=0;
		endOfBlockTx=0;
		while(sentTextCount<textLength)
		{
			remainTextCount=textLength-sentTextCount;
			if(remainTextCount>5)
				sendBlock(5,&sentTextCount,0);
			else
				sendBlock(remainTextCount, &sentTextCount,1);
			while(getACK==1);
		}
	}
}

