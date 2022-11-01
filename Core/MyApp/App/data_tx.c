/*
 * data.c
 *
 *  Created on: 11 okt. 2022
 *      Author: Tom Selier
 */
#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

extern unsigned int Samplerate;

/**
 * @brief receives data from the CharQ, sent by UART keys.
 * Converts the data from characters to bits, then sends data to bitQ
 */
void Prep_data_task()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char CharBuf[QSIZE_DATA];
	char BitBuf[QSIZE_DATA*8];
	char* p;
	int pos = 0;
	int length;

	while(TRUE)
	{
		// wacht tot er daadwerkelijk data binnenkomt
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		pos = 0;

		// leeg de character buf in voorbereiding tot data
		memset(CharBuf, 0, QSIZE_DATA);

		// CharBuf vullen
		while(xQueueReceive(hChar_Queue, (void *) &CharBuf[pos], (TickType_t) 0))
			pos++;

		// lengte van de inkomende data opslaan
		length = pos;

		// data van char naar buf
		Char_to_bits(BitBuf, CharBuf, length);

		// pointer naar [0]
		p = BitBuf;

		// verstuur alle bits naar de q
		for(pos = 0; pos < length*8; p++, pos++)
			xQueueSend(hBit_Queue, p, 0);

		// debug
		if(pdFALSE)
		{
			for(pos = 0; pos < length; pos++)
			{
				UART_putchar(CharBuf[pos]);
				UART_puts("\n");
			}
		}
	}
}


/**
 * @brief Verstuurd alle bits in blokjes van 64 bits vanuit de bitQ
 * Non-blocking
 *
 * Mogelijk later vervangen door een interrupt met timer,
 * zeker als SAMPLE_RATE korter wordt of als dit niet betrouwbaar genoeg is
 */
void Send_data_task()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char BitBuf[QSIZE_DATA];
	int i, length;
	char SOC[] =	{1, 1, 1, 1, 1,
					1, 1, 1, 1,
					1, 1, 1, 1,
					1, 1, 1, 0}; // 0xFE of 254

	int amountWaiting = 0;
	char ETX = 	0x03;
	char EOT = 	0x04;


	while(TRUE)
	{
		// tijdelijke 2 sec delay zodat je meerdere datablokken hoort
		// uiteindelijk kan deze veel korter
		osDelay(100);

		// opslaan of er nog data in de Q zit
		amountWaiting = uxQueueMessagesWaiting(hBit_Queue);

		// skip de task als de Q leeg is
		if(amountWaiting == 0)
			continue;

		// reset iterator
		i = 0;

		// haal bits uit de Q zolang er bits inzitten EN i minder is dan 64
		while(xQueueReceive(hBit_Queue, (void *) &BitBuf[i], (TickType_t) 0) && i < QSIZE_DATA-1)
			i++;

		// lengte opslaan in het geval dat er niet precies 64 bits in de Q zaten
		length = i;

		// ledje aan om transmit te tonen aan user
		HAL_GPIO_TogglePin(GPIOD, LEDRED);

		// Start de speaker
		Toggle_Speaker(START);

//		Change_Frequency(FREQHIGH);
//		osDelay(SAMPLERATE);
		// SOC sturen
		for(i = 0; i < sizeof(SOC); i++)
		{
			if(SOC[i])
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// verstuur de bits met een snelheid van samplerate
		for(i = 0; i <=length; i++)
		{
			if(BitBuf[i])
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// check opnieuw of de Q leeg is
		amountWaiting = uxQueueMessagesWaiting(hBit_Queue);

		// lengte aanvullen met NULL als dat nodig is
		for(; i < 64; i++)
		{
			Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// ETX sturen als er nog berichten in de Q zitten
		for(i = 7; i >= 0 && amountWaiting != 0; i--)
		{
			if(((ETX >> i) & 0x01) == 1)
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}


		// EOT sturen als de Q leeg is
		for(i = 7; i >= 0 && amountWaiting == 0; i--)
		{
			if(((EOT >> i) & 0x01) == 1)
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// Stop de speaker
		Toggle_Speaker(STOP);

		// zet t ledje weer uit als we klaar zijn
		HAL_GPIO_TogglePin(GPIOD, LEDRED);
		UART_puts("\nNew transmission");
	}
}


/**
 * @brief functie die char array omzet naar bits array
 * @param char* bit array, target
 * @param char* char array, source
 * @param int 	lengte van de CHAR array
 */
void Char_to_bits(char* BitTarget, char* CharSource, int length)
{
	int i, j, k = 0;

	for(j = 0; j < length; j++) // per letter
	{
		i = 8*(j+1)-1; // start per letter op 7, 15, 23 etc
		for(; k < 8; i--, k++) // per bit
		{
			BitTarget[i] = (CharSource[j] >> k) & 0x01;
		}
		k = 0;
	}

	if(pdTRUE) // debug
	{
		UART_puts("\n");
		for(i = 0; i < length*8; i++)
		{
			UART_putint((int) BitTarget[i]);
			if((1+i)%8==0)
			{
				UART_puts(" ");
				UART_putchar(CharSource[((i+1)/8)-1]);
				UART_puts("\n");
			}
		}
	}
}
