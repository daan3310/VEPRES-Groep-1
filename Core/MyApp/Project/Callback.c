/*
 * Callback.c
 *
 *  Created on: Oct 6, 2022
 *      Author: stein
 */

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"


/**
* @brief:	Callback routine voor external interrupts
* @param:	GPIO_Pin, meegegeven triggerpin
* @return: 	void
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_7)
	{
		HAL_GPIO_TogglePin(GPIOD, LEDORANGE);
	}

	if(GPIO_Pin == GPIO_PIN_10)
	{
		Period_time();
	}
}
