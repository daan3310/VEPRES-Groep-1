/**
* @file 	Sample.c
* @brief 	Hierin worden twee dingen afgehandeld: synchronisatie en sampling van signaal
*			De synchronisatie gaat dmv EXTI die gecalled wordt bij rising edges op binnenkomend signaal
*			Zodra er genoeg rising edges gevonden zijn binnen bepaalde tijd van elkaar triggered de sync
*			De samplingtimer wordt dan gestart en die sampled om een gegeven tijd het binnenkomend signaal voor data
* @author 	Stein van Vliet

* @date 	29/09/2022
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
uint8_t end = 0;
uint8_t rec = 0;

unsigned long Previous = 0;
unsigned long Uptime = 0;
unsigned int Samplerate = 20;

int flag = 0;
int TCycle = 0;

static char *speak = "\nI counted: ";

/**
* @brief: 	Leest pulsecount uit timer register
* @param: 	hSample_Timer, voor het geval je de timer handle wilt aanpassen
* @return: 	void
*/
void Sample_Handler(TimerHandle_t hSample_Timer)
{
	int data =0;

	uint32_t Sample = 0;

	//Lees timerregister uit voor aantal pulsen binnenin sampletijd
	Sample = (TIM2->CNT);
	TIM2->CNT=0;

	//Als sample boven hoge frequentiegrens dan data=1
	if(Sample>(Upper-(Upper/SPACING)))
		data = 1;
	//Als sample onder lage frequentiegrens dan data=0
	else if(Sample>(Lower-(Lower/SPACING)))
		data = 0;
	//Anders data=7, arbitraire afgesproken waarde (levert 253 als alle bits in een byte 7 zijn)
	else
	{
		data = 7;
		stop++;		//Verhoog stopwaarde
	}

	//Schrijf de data naar 1 byte
	buf ^= data << (7-TCycle);

	//Debug
	if((Uart_debug_out & SAMPLE_DEBUG_OUT) )
	{
		UART_puts(speak);
		UART_putint(Sample);
		UART_puts(" data: ");
		UART_putint(data);
		UART_puts(" TCycle: ");
		UART_putint(TCycle);
	}

	//Als de sample op de laatste cycle is
	if(TCycle >6)
	{
		//Debug, negeer byte als gevuld met alleen 7's
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
		TCycle++; 	//Verhoog cyclecount

	//Check stopvoorwaarde
	if(stop>(Upper-(Upper/SPACING)))
	{
		stop = 0;
		first = 0;
		TCycle = 0;
		xTimerStop(hSample_Timer,portMAX_DELAY);
	}
}

/**
* @brief: 	Checkt de meegegeven byte voor functie
* @param: 	byte, byte voor de check
* @return: 	void
*/
void Msg_check(uint8_t byte)
{
	if(end==1)
	{
		UART_puts(" CRC\n");
		xTimerStop(hSample_Timer,portMAX_DELAY);
		xQueueSend(mBit_Queue,&byte,portMAX_DELAY);
		xTaskNotifyGive(hData_name);
		end =0;
	}
	else
	{
		//Onderzoekt byte voor speciale functie
		switch(byte)
		{
		case 0x02:	//SOT
			UART_puts(" Start of text");
			rec =1;
			break;
		case 0x03:	//ETX
			UART_puts(" End of text");
			rec =0;
			end =1;
			break;
		case 0x04:	//EOT
			UART_puts(" End of transmission");
			end =1;
			rec =0;
			break;
		//Bij default gooi de byte in de queue
		default:
			if(rec==1)
				xQueueSend(mBit_Queue,&byte,portMAX_DELAY);
			break;
		}
	}
}

/**
* @brief: 	Callback functie die pulsen van bepaalde periodetijd telt voor sync
* @param: 	void
* @return: 	void
*/
void Period_time(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned long Current = HAL_GetTick();
	unsigned long Dif = Current - Previous;

	//Registreer tijd verstreken sinds vorige call
	Previous = Current;

	//Lees timerregister uit voor periodetijd
	Period = (TIM4->CNT);
	TIM4->CNT=0;

	//Doorloopt de states van de functie
	switch(flag)
	{
		//Case 0, deze case zoekt naar 15 pulsen hoog van sync
		case 0:

			//Check of periodetijd van hoge frequentie
			if(Period > 320 && Period < 380)
			{
				Startsign++;
			}
			else if(Dif>10)
			{
				UP = 0;
				Startsign = 0;
				//Errorled verkeerde periodetijd of tijd verstreken
				HAL_GPIO_TogglePin(GPIOD, LEDBLUE);
			}

			//Aantal pulsen voor sample hoge frequentie
			if(Startsign >= Upper)
			{
				UP++;
				Startsign = 0;
				//Debugled voor sample hoge frequentie
				HAL_GPIO_TogglePin(GPIOD, LEDRED);
			}

			//Safety measure voor omvallen sync
			if((UP)>3&&(Period>400))
			{
				UP=0;
			}

			//Aantal samples hoge frequentie boven grens
			if(UP > 14)
			{
				Uptime = HAL_GetTick();
				flag = 1;
				Startsign = 0;
				//Debugled voor 15 pulsen hoog gevonden
				HAL_GPIO_TogglePin(GPIOD, LEDGREEN);
			}
			break;

		//Case 1, check of een puls laag volgt en (her)start de timer
		case 1:
			//Timer safety en check periodetijd
			if(((Current-Uptime)<50)&&(Period > 400 && Period < 500))
				Startsign++;
			else
			{
				UP = 0;
				Startsign = 0;
				flag = 0;
			}

			//Aantal pulsen voor sample lage frequentie
			if(Startsign >= Lower)
			{
				//Reset sampletimer register
				TIM2->CNT = 0;
				UP = 0;
				Startsign = 0;
				flag = 0;

				//Debug
				if(Uart_debug_out & SAMPLE_DEBUG_OUT)
					UART_puts("\r\nSync found Timer reset!");

				//Als timer uitstaat zet hem aan
				if(first==0)
				{
					xTimerStartFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					first=1;
					buf=0;
				}
				//Anders reset de timer
				else
				{
					xTimerResetFromISR(hSample_Timer,xHigherPriorityTaskWoken);
					TCycle = 0;
					buf=0;
				}

				//Debugled voor complete synchronisatie
				HAL_GPIO_TogglePin(GPIOD, LEDORANGE);

			}

			break;
	}
}

/**
* @brief: 	Schrijft dynamische waarden voor periodetijden uit bij opstart
* @param: 	speed, meegegeven sampletijd in miliseconden
* @return: 	void
*/
void Speed_init(int speed)
{
	Stime = 1000/speed;
	Upper = 2800/Stime;
	Lower = 2200/Stime;
	Samplerate = speed;
}

/**
* @brief: 	Schrijft dynamische waarden voor periodetijden en verandert timerperiode
* @param: 	speed, meegegeven sampletijd in miliseconden
* @return: 	void
*/
void Speed_calc(int speed)
{
	 Stime = 1000/speed;
	 Upper = 2800/Stime;
	 Lower = 2200/Stime;
	 Samplerate = speed;
	 xTimerChangePeriod(hSample_Timer,speed,portMAX_DELAY);
}

