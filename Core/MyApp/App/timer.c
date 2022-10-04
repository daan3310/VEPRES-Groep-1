/**
* @file timer.c
* @brief Demonstreert: timerhandler.
* De timer zelf wordt aangemaakt in handles.c
*
* @author MSC
*
* @date 5/5/2022
*/
#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"



/**
* @brief Vangt de FreeRTOS software-interrupt op (zie handles.c), en toggelt een ledje
* @param hTimer1 De handle van de timer
* @return void
*/
void Timer1_Handler(TimerHandle_t hTimer1)
{
	HAL_GPIO_TogglePin(GPIOD, LEDBLUE);   // turns led on/off
}

