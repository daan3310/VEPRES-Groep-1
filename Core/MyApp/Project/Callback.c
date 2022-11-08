/**
* @file 	Callback.c
* @brief 	Hier wordt de STM EXTI Callbackfunctie herschreven voor aanroepen van gemaakte functies
*
* @author 	Stein van Vliet

* @date 	06/10/2022
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
	//callback voor external testpin
	if(GPIO_Pin == GPIO_PIN_7)
	{
		HAL_GPIO_TogglePin(GPIOD, LEDORANGE);
	}

	//callback voor external synchronisatiepin
	if(GPIO_Pin == GPIO_PIN_10)
	{
		Period_time();
	}
}
