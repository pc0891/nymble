#include "serialport.h"
#include "text.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

//function to wait for MCU to singal it is ready to receive again
char waitTillMCUready(HANDLE hFile)
{
	char controlFlag, loopCount;
	
	loopCount=0;
	
	do{
		if((readFromSerialPort(hFile,&controlFlag,1))==1)
			if(controlFlag==17)
				break;
		loopCount++;
	}while(loopCount<100);
	
	if(loopCount<100)
		return 1;
	else
		return 0;
}

//function to send acknowledgement to MCU that block of characters has been received and it can transmit more characters
char sendACK(HANDLE hFile)
{
	char ch=17;
	if((writeToSerialPort(hFile,&ch,1))==1)
		return 1;
	else
		return 0;
}

int main()
{
	int bytesWritten;
	int bytesRead; 
	int textCount, bufferCount;
	char sendBuffer[5];
	char recBuffer[10];
	char recText[1024];
	char endOfText;
	clock_t startTime, endTime;
	double duration, dataRate;
	
	HANDLE pcAVRcom=openSerialPort("COM4",B2400,one,off); //open COM port
	
	textCount=0;
	endOfText=0;
	bytesWritten=0;
	
	startTime=clock(); //record start time of transmission
	
	printf("Nymble Assignment Demo\n");
	printf("Data transfer rate below\n");
	
	do
	{
		//send text in blocks of 5 characters
		for(bufferCount=0;bufferCount<5;bufferCount++)
		{
			if(text[textCount]!='\0')
				sendBuffer[bufferCount]=text[textCount++];
			else
			{
				endOfText=1;
				break;
			}
		}
		if(endOfText==1)
			sendBuffer[bufferCount++]=3;
		else
			sendBuffer[bufferCount++]=4;
		bytesWritten+=writeToSerialPort(pcAVRcom,sendBuffer,bufferCount);
		
		//display data transfer rate every 0.5 seconds
		endTime=clock();
		duration=((double)(endTime-startTime))/CLOCKS_PER_SEC;
		if((duration>=0.5))
		{
			dataRate=(bytesWritten*8)/duration;
			printf("\r%0.2f",dataRate);
			fflush(stdout);
		}
		
		//wait till MCU is ready to receive another block of text
		if(waitTillMCUready(pcAVRcom)==0)
		{
			printf("MCU ACK Fail\n");
			if(!closeSerialPort(pcAVRcom))
				printf("Error: Closing Port failed");
			return 0;
		}
	}while(endOfText==0);
	
	endOfText=0;
	bytesRead=0;
	textCount=0;

	startTime=clock(); //record start of reception
	
	do
	{
		bytesRead+=readFromSerialPort(pcAVRcom,recBuffer,6);
		
		//display data transfer rate every 0.5 seconds
		endTime=clock();
		duration=((double)(endTime-startTime))/CLOCKS_PER_SEC;
		if((duration>=0.5))
		{
			dataRate=(bytesRead*8)/duration;
			printf("\r%0.2f",dataRate);
			fflush(stdout);
		}
		
		for(bufferCount=0;bufferCount<6;bufferCount++)
		{
			switch(recBuffer[bufferCount])
			{
				case 4:   //received end of block character
					if(sendACK(pcAVRcom)==0)  //send acknowledgment that block of text successfully received
					{
						printf("Send ACK Failed\n");
						if(!closeSerialPort(pcAVRcom))
							printf("Error: Closing Port failed");
						return 0;
					}
					break;
				case 3:  //received end of text character
					if(sendACK(pcAVRcom)==0)  //send acknowledgement that entire text successfully received
					{
						printf("Send ACK Failed\n");
						if(!closeSerialPort(pcAVRcom))
							printf("Error: Closing Port failed");
						return 0;
					}
					endOfText=1;
					break;
				default:
					recText[textCount++]=recBuffer[bufferCount];
			}
		
		if(endOfText==1)
			break;
		}
	}while(endOfText==0);
	
	recText[textCount]='\0';
	printf("\n%s\n",recText); // print the received text from MCU
	

	if(!closeSerialPort(pcAVRcom)) //close COM port
			printf("Error: Closing Port failed");	
	return 0;
}
