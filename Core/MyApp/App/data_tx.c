/*
 * data.c
 *
 *  Created on: 11 okt. 2022
 *      Author: Tom Selier
 */
#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

void Send_data_task()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char buf[QSIZE_DATA];
	int pos = 0;
	int length;

	while(TRUE)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		pos = 0;
		memset(buf, 0, QSIZE_DATA);

		while(xQueueReceive(hData_Queue, (void *) &buf[pos], (TickType_t) 0))
		{
			pos++;
		}

		length = pos;

		if(0) // debug
		{
			for(pos = 0; pos < length; pos++)
			{
				UART_putchar(buf[pos]);
				UART_puts("\n");
			}
		}

		String_to_bits(buf, length);
	}
}


/**
 * @brief converts a string to a string of bits in big endian
 * @param char* pointer to string
 * @param int length of string
 * @return void
 */
void String_to_bits(char* p, int length)
{
	unsigned char arrayChar[length]; // tijdelijke opslag characters
	unsigned char arrayBit[length*8]; // tijdelijke opslag bits
	int i, j, k = 0;

	for(i = 0; i < length; i++) // string overzetten in een array
	{
		arrayChar[i] = p[i];
//		UART_putchar(arrayChar[i]);
	}

	// verstuurd BIG endian
	for(j = 0; j < length; j++) // per letter
	{
		i = 8 * (j + 1) - 1; // start per letter op 7, 15, 23 etc
		for(; k < 8; i--, k++) // per bit
		{
			arrayBit[i] = (arrayChar[j] >> k) & 0x01;
		}
		k = 0;
	}

	if(0) // debug
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


	HAL_GPIO_TogglePin(GPIOD, LEDRED);

	for(i = 0; i < length*8; i++) // maak geluid
	{
		if(arrayBit[i])
			Change_Frequency(FREQHIGH);
		else
			Change_Frequency(FREQLOW);

		vTaskDelay(SAMPLERATE);
	}

	HAL_GPIO_TogglePin(GPIOD, LEDRED);
}




