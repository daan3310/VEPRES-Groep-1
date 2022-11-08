/*
 * speaker.c
 *
 * Created on: Oct 5, 2022
 * Author: 	Tom Selier
 */
#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

unsigned int toggle = 0;
extern unsigned int Samplerate;

/**
 * @brief zet de speaker aan of uit
 * @param START = aan, STOP = uit
 * @return void
 */
void Toggle_Speaker(int state)
{
	// START
	if(state)
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

	// STOP
	else
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}

/**
 * @brief initialiseert de timer voor het gebruik van de speaker
 * @param
 * @return void
 */
void Speaker_Init()
{
	TIM3->ARR = 1000-1;
	TIM3->CCR3 = 500-1;
}

/**
 * @brief Veranderd de pwm frequentie van timer 3 op pin pc8 (buzzer)
 * @param int De gewenste frequentie in Hz
 * @return void
 */
void Change_Frequency(int frequency)
{
	// verander de pwm frequentie
	TIM3->ARR 	= 1000000 / frequency - 1;

	// duty cycle altijd op 50%
	TIM3->CCR3 	= 1000000 / (frequency * 2) - 1;

	// zet counter op 0 om te voorkomen dat de counter groter is
	// dan de register waarde
	TIM3->CNT = 0;

	// debug werkt alleen bij operationele frequenties
	// 2200 Hz of 2800 Hz
	if(Uart_debug_out & SEND_DEBUG_OUT)
	{
		UART_puts("\r\n");

		if(frequency == FREQHIGH)
			UART_putint(1);
		else if(frequency == FREQLOW)
			UART_putint(0);
		else
			UART_puts("Change_Frequency: Frequentie veranderd naar niet standaard frequentie.");
	}
}

/**
 * @brief Toggled tussen 2200 en 2800 Hz, gebruikt Change_Frequency()
 * @return void
 */
void Toggle_Frequency()
{
	if(toggle)
	{
		Change_Frequency(FREQHIGH);
		UART_puts("\r\nFrequency toggled: ");
		UART_putint(FREQHIGH);
	}
	else
	{
		Change_Frequency(FREQLOW);
		UART_puts("\r\nFrequency toggled: ");
		UART_putint(FREQLOW);
	}

	toggle = !toggle;
}

/**
 * DEPRECATED
 * @brief toggles between high and low bytes*samplerate times, blocking
 * @param int amount of bytes to transmit
 * @return void
 * DEPRECATED
 */
void Sync_Bytes(int bytes)
{
	int i;
	char SOC[] =	{0, 1, 1, 1,
					1, 1, 1, 0}; // 0xFE of 254

	UART_puts("\r\nRunning Syncbytes");
	toggle = 0; // always start at 0


	Toggle_Speaker(START);
	for(i = 0; i < sizeof(SOC); i++)
	{
		if(SOC[i])
			Change_Frequency(FREQHIGH);
		else
			Change_Frequency(FREQLOW);
		osDelay(Samplerate);
	}
	Toggle_Speaker(STOP);
}

