/**
* @file data_rx.c
* @brief hier worden bits omgezet naar characters
*
* @author daniel roling

* @date 17/10/2022
*/
#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"


/**
* @brief Oefentask voor studenten
* @param argument, kan evt vanuit tasks gebruikt worden
* @return void
*/
void DataRx1 ()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char buf[80];
	unsigned int i = 0;
	int k = 0;
	char eot = 0;
	//int lengte = 40;
	int letter = 0;
	char bitBericht[8];
	while(TRUE)
	{
		osDelay(1000);
		//lengte = uxQueueMessagesWaiting(mBit_Queue)/8;
		if(uxQueueMessagesWaiting(mBit_Queue)%8 == 0 && uxQueueMessagesWaiting(mBit_Queue) != 0)
		// kijkt of er een even aantal bits in de queue zit en er iets in de queue zit
		{
			k = 0;
			while(xQueueReceive(mBit_Queue, (void *) &bitBericht[k], (TickType_t) 0) && k < 7)
			// Vult 8 bits in een array om te veranderen naar een char
				k++;

			for (i=0; i < 8 ; i++)
			{
				if(bitBericht[i] == 1)
					letter += (pow(2, (7-i)));
			}
			if(letter == 4)
				eot = 1;
			if(letter != 0 && eot == 0)
				UART_putchar(letter);
			letter = 0;
		}
		//UART_puts("\n");
		//UART_puts(charBericht);

		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
	       	sprintf(buf, "\r\n%s: %u", __func__, i++);
			UART_puts(buf);
    	}
	}
}


