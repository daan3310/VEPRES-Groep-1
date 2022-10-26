/*
 * Counter.c
 *
 *  Created on: 29 Sep 2022
 *      Author: stein
 */

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"

int TCycle = 0;
uint8_t Startsign = 0;
uint16_t Period = 0;
uint8_t UP = 0;
uint8_t buf = 0;

uint8_t first = 0;
uint8_t reset = 0;
uint8_t stop = 0;

unsigned long Previous = 0;
unsigned long Uptime = 0;

int flag = 0; // test case Tom

static char *speak = "\nI counted: ";

/**
* @brief Leest pulsecount uit timer register
* @return void
*/
void Sample_Handler(TimerHandle_t hSample_Timer)
{
//	HAL_GPIO_TogglePin(GPIOD, LEDORANGE);
	int data =0;

	uint32_t Sample = 0;

	Sample = (TIM2->CNT);
	TIM2->CNT=0;

	if(Sample>50)
		data = 1;
	else if(Sample>40)
		data = 0;
	else
	{
		data = 7;
		stop++;
	}



	buf ^= data << (7-TCycle);

	if((Uart_debug_out & SAMPLE_DEBUG_OUT) )
	{
		UART_puts(speak);
		UART_putint(Sample);
		UART_puts(" data: ");
		UART_putint(data);
		UART_puts(" TCycle: ");
		UART_putint(TCycle);
	}

	if(TCycle >6)
	{
		xEventGroupSetBits(hSample_Event, buf);
		if((Uart_debug_out & SAMPLE_DEBUG_OUT) && buf!=253)
		{
			UART_puts("\nbyte received: ");
			UART_putint(buf); UART_putchar(buf);
		}
		TCycle = 0;
		buf = 0;
	}
	else
		TCycle++;

	if(stop>50)
		{
			stop = 0;
			first = 0;
			TCycle = 0;
			xTimerStop(hSample_Timer,portMAX_DELAY);
		}
}

void Period_time(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned long Current = HAL_GetTick();
	unsigned long Dif = Current - Previous;
	Previous = Current;

	Period = (TIM4->CNT);
	TIM4->CNT=0;

//	if(Dif>20)
//	{
//		flag = 0;
//		Startsign = 1;
//		UP = 0;
//	}

	switch(flag)
	{
		case 0:
			if(Period > 320 && Period < 380)
			{
				Startsign++;
			}
			else if(Dif>10)
			{
				UP = 0;
				Startsign = 0;
				HAL_GPIO_TogglePin(GPIOD, LEDBLUE);
			}

			if(Startsign >= 56)
			{
				UP++;
				Startsign = 0;
				HAL_GPIO_TogglePin(GPIOD, LEDRED);
			}

			if((UP)>3&&(Period>400))
			{
				UP=0;
			}

			if(UP > 14)
			{
				Uptime = HAL_GetTick();
				flag = 1;
				Startsign = 0;
				HAL_GPIO_TogglePin(GPIOD, LEDGREEN);
			}

			break;

		case 1:
			if((Current-Uptime)<50)
			{
				if(Period > 400 && Period < 500)
				{
					Startsign++;
				}
			}
			else
			{
				UP = 0;
				Startsign = 0;
				flag = 0;
			}

//			if(Startsign>30)
//			{
//				flag =1;
//			}

			if(Startsign >= 44)
			{
				TIM2->CNT = 0;
				//TCycle = 0;
				UP = 0;
				Startsign = 0;
				flag = 0;
//				UART_puts("\r\nSync found Timer reset!");
				if(first==0)
				{
					xTimerStartFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					xTimerResetFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					first=1;
				}
				else
				{
					xTimerResetFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					reset = 1;
					TCycle = -1;
					buf=0;
				}

				HAL_GPIO_TogglePin(GPIOD, LEDORANGE);

			}

			break;


	}

}


