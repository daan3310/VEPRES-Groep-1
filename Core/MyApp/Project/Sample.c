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

#define SPACING 7

uint8_t Startsign = 0;
uint16_t Period = 0;
uint8_t UP = 0;
uint8_t buf = 0;

uint8_t Stime;
uint8_t Upper;
uint8_t Lower;

uint8_t first = 0;
uint8_t reset = 0;
uint8_t stop = 0;

uint8_t rec = 0;
//uint8_t

unsigned long Previous = 0;
unsigned long Uptime = 0;

unsigned int Samplerate = 20;

int flag = 0; // test case Tom
int TCycle = 0;

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

	if(Sample>(Upper-(Upper/SPACING)))
		data = 1;
	else if(Sample>(Lower-(Lower/SPACING)))
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
//		xEventGroupSetBits(hSample_Event, buf);
		if(buf!=253)
		{
			UART_puts("\nbyte received: ");
//			UART_putint(buf);
			UART_putchar(buf);
			Msg_check(buf);
		}
		TCycle = 0;
		buf = 0;
	}
	else
		TCycle++;

	if(stop>(Upper-(Upper/SPACING)))
		{
			stop = 0;
			first = 0;
			TCycle = 0;
			xTimerStop(hSample_Timer,portMAX_DELAY);
		}
}

void Msg_check(uint8_t byte)
{
	switch(byte)
	{
	case 0x02:	//SOT
		UART_puts(" Start of text");
		rec =1;
		break;
	case 0x03:	//ETX
		UART_puts(" End of text");
		rec =0;
		break;
	case 0x04:	//EOT
		UART_puts(" End of transmission");
		xTimerStop(hSample_Timer,portMAX_DELAY);
		rec =0;
		xTaskNotifyGive(hData_name);
		break;
	default:
		if(rec==1)
			xQueueSend(mBit_Queue,&byte,portMAX_DELAY);
		break;
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

			if(Startsign >= Upper)
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

			if(Startsign >= Lower)
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
//					xTimerResetFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					first=1;
					buf=0;
				}
				else
				{
					xTimerResetFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					TCycle = 0;
					buf=0;
				}

				HAL_GPIO_TogglePin(GPIOD, LEDORANGE);

			}

			break;
	}
}

void Speed_init(int speed)
{
	Stime = 1000/speed;
	Upper = 2800/Stime;
	Lower = 2200/Stime;
	Samplerate = speed;
}

void Speed_calc(int speed)
{
	 Stime = 1000/speed;
	 Upper = 2800/Stime;
	 Lower = 2200/Stime;
	 Samplerate = speed;
	 xTimerChangePeriod(hSample_Timer,speed,portMAX_DELAY);
}

