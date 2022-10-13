/*
 * data.c
 *
 *  Created on: 11 okt. 2022
 *      Author: Tom Selier
 */
#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

void String_to_bits(char* p, int length)
{
	unsigned char arrayChar[length]; // tijdelijke opslag characters
	unsigned char arrayBit[length*8]; // tijdelijke opslag bits
	int i, j, k = 0;

	for(i = 0; i < length; i++) // string overzetten in een array
	{
		arrayChar[i] = p[i];
		UART_putchar(arrayChar[i]);
	}


	for(j = 0; j < length; j++) // per letter
	{
		i = 8*(j+1)-1;
		for(; k < 8; i--, k++) // per bit
		{
			arrayBit[i] = (arrayChar[j] >> k) & 0x01;
		}
		k=0;
	}

	if(1) // debug
	{
		UART_puts("\n");
		for(i = 0; i < length*8; i++)
		{
			UART_putint((int) arrayBit[i]);
			if((1+i)%8==0)
			{
				UART_puts("\n");
			}
		}
	}


	for(i = 0; i < length*8; i++) // maak geluid
	{
		if(arrayBit[i])
			Change_Frequency(FREQHIGH);
		else
			Change_Frequency(FREQLOW);

		vTaskDelay(SAMPLERATE);
	}
}




