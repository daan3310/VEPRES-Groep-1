/**
* @file microfoon.c
* @brief Hier kunnen studenten hun eigen tasks aanmaken
*
* @author MSC

* @date 5/5/2022
*/
#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"
uint32_t Sample = 0;
uint8_t Cycle = 0;
uint8_t buf = 0;

/**
* @brief Oefentask voor studenten
* @param argument, kan evt vanuit tasks gebruikt worden
* @return void
*/
void Sample_Handler(TimerHandle_t hSample_Timer)
{
	HAL_GPIO_TogglePin(GPIOD, LEDORANGE);
	int data =0;

	Sample = (TIM2->CNT);
	TIM2->CNT=0;

	if(Sample>=50)
		data = 1;
	else
		data = 0;
	if(Sample > 10)
	xQueueSend(mBit_Queue, &data, 0);

}


