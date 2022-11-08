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
void Prep_data_task(void *argument)
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

		// pos resetten
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
		if(Uart_debug_out & TX_DEBUG_OUT)
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
void Send_data_task(void *argument)
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char BitBuf[QSIZE_DATA];
	int i, length;
	char SOC[] =	{1, 1, 1, 1, 1,
					1, 1, 1, 1,
					1, 1, 1, 1,
					1, 1, 1, 0}; // 0xFE of 254

	int amountWaiting = 0;
	char SOT = 	0x02;
	char ETX = 	0x03;
	char EOT = 	0x04;
	char CrC;

	while(TRUE)
	{
		// 100ms delay om rx tijd te geven data te verwerken
		osDelay(100);


		// skip de task als de Q leeg is
		if(uxQueueMessagesWaiting(hBit_Queue) == 0)
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

		// SOC sturen
		for(i = 0; i < sizeof(SOC); i++)
		{
			if(SOC[i])
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		for(i = 7; i >= 0; i--)
		{
			if(((SOT >> i) & 0x01) == 1)
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// verstuur de bits met een snelheid van samplerate
		for(i = 0; i <= length; i++)
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
			//BitBuf aanvullen met 0 voor CRC berekeningen
			BitBuf[i] = 0;

			if(BitBuf[i])
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}


		// Als er nog iets in de Q staat, stuur ETX
		if(amountWaiting != 0)
		{
			// ETX sturen als er nog berichten in de Q zitten
			for(i = 7; i >= 0; i--)
			{
				if(((ETX >> i) & 0x01) == 1)
					Change_Frequency(FREQHIGH);
				else
					Change_Frequency(FREQLOW);
				osDelay(Samplerate);
			}
		}

		// Als er niks meer in de Q staat, stuur EOT
		if(amountWaiting == 0)
		{
			for(i = 7; i >= 0; i--)
			{
				if(((EOT >> i) & 0x01) == 1)
					Change_Frequency(FREQHIGH);
				else
					Change_Frequency(FREQLOW);
				osDelay(Samplerate);
			}
		}

		//Voor het geval er 8 bytes meekomen, corrigeer de lengte van bitarray
		if(length==63)
			length++;

		// Bouw CRC op basis van BitBuf en lengte
		CrC = CRC_Builder(BitBuf,length);

		// Stuur CRC mee
		for(i = 7; i >= 0; i--)
		{
			if(((CrC >> i) & 0x01) == 1)
				Change_Frequency(FREQHIGH);
			else
				Change_Frequency(FREQLOW);
			osDelay(Samplerate);
		}

		// Stop de speaker
		Toggle_Speaker(STOP);

		// zet t ledje weer uit als we klaar zijn
		HAL_GPIO_TogglePin(GPIOD, LEDRED);

		//Debug
		if(Uart_debug_out & TX_DEBUG_OUT)
			UART_puts("\nNew transmission");
	}
}


/**
 * @brief functie die char array omzet naar bits array, output in big endian
 * @param char* bit array, target
 * @param char* char array, source
 * @param int 	lengte van de CHAR array
 */
void Char_to_bits(char* BitTarget, char* CharSource, int length)
{
	// i iterator bit target te vullen
	// j iterator char source bij te houden
	// k iterator bit shift
	int i, j, k = 0;

	 // Per letter door CharSource
	for(j = 0; j < length; j++)
	{
		// start per letter op 7, 15, 23 etc
		i = 8*(j+1)-1;

		// per bit uitlezen
		for(; k < 8; i--, k++)
			BitTarget[i] = (CharSource[j] >> k) & 0x01;

		k = 0;
	}

	if(Uart_debug_out & TX_DEBUG_OUT) // debug
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
