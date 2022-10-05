/*
 * speaker.c
 *
 *  Created on: Oct 5, 2022
 *      Author: Tom Selier
 */
#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

unsigned int toggle = 0;


void Speaker_Init()
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	TIM3->ARR = 1000-1;
	TIM3->CCR3 = 500-1;
}

/**
 * @brief Veranderd de frequentie van timer 3 op pin pc8 (buzzer)
 * @param int De gewenste frequentie
 * @return void
 */
void Change_Frequency(int frequency)
{
	TIM3->ARR 	= 1000000 / frequency - 1;
	TIM3->CCR3 	= 1000000 / (frequency * 2) - 1;
}

/**
 * @brief Toggled tussen 2200 en 2800 Hz, gebruikt Change_Frequency()
 * @return void
 */
void Toggle_Frequency()
{
	int high = 2800;
	int low = 2200;

	if(toggle)
	{
		Change_Frequency(high);
		UART_puts("\r\nFrequency toggled: ");
		UART_putint(high);
	}
	else
	{
		Change_Frequency(low);
		UART_puts("\r\nFrequency toggled: ");
		UART_putint(low);
	}

	toggle = !toggle;
}

