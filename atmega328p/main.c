/*
 * serialInt.c
 *
 * Created: 11/04/2024 10:55:04
 * Author : pramod
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "serial.h"

//program to transmit text to PC in blocks of 5 characters
void sendBlock(int length, int* textCount, char endOfText)
{
	eeprom_busy_wait();
	eeprom_read_block((void*)bufferTx,(const void*)*textCount,length);
	eeprom_busy_wait();
	if(endOfText==1)
		bufferTx[length]=3;  //control character to signify end of text
	else
		bufferTx[length]=4; // control character to signify end of transmit block 
	count=0;
	getACK=1;
	UDR0=bufferTx[0];
	while(endOfBlockTx==0);
	(*textCount)+=length;
}


int main(void)
{
    int textLength,sentTextCount, remainTextCount;
	
	uartInit(); //intialize UART interrupt
	sei();
	
	
	while(1)
	{
		textLength=0;
		endOfRx=0;
		endOfBlockRx=0;
		count=0;
		
		while(endOfRx==0)
		{
			while(endOfBlockRx==0); //wait till a block of characters are received
			eeprom_busy_wait();
			eeprom_write_block((const void*)bufferRx,(void*)textLength,count); //store in received block of text in EEPROM
			textLength+=count; //stores count of characters received. At end of reception signifies text length received
			UDR0=17;
			sendACK=1;
			while(endOfBlockRx==1);
		}
		
		sentTextCount=0;
		endOfBlockTx=0;
		while(sentTextCount<textLength)
		{
			remainTextCount=textLength-sentTextCount;
			if(remainTextCount>5) //send text in blocks of 5
				sendBlock(5,&sentTextCount,0);
			else
				sendBlock(remainTextCount, &sentTextCount,1); //send remaining text if text length not multiple of 5
			while(getACK==1);
		}
	}
}

