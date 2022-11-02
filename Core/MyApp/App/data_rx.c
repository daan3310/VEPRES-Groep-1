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
* @ zet de ontvangen bits om in letters om te printen
* @return void
*/
void DataRx()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	//char buf[80];
//	char crcBuffer[8];
	//unsigned int i = 0;
	char letter;
	//char EOT = 0;
	//int lengte = 40;
	uint8_t byteBericht[65];
	while(TRUE)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		UART_puts("DataRX unblocked");

//		if(uxQueueMessagesWaiting(mBit_Queue) == 0)
//			continue;
		//lengte = uxQueueMessagesWaiting(mBit_Queue)/8;
//		if((uxQueueMessagesWaiting(mBit_Queue) != 0))
//		// kijkt of er een even aantal bits in de queue zit en er iets in de queue zit
//		{
//			while(xQueueReceive(mBit_Queue, (void *) &letter, (TickType_t) 0))
//					UART_putchar(letter);



//			k = 0;
//			while(xQueueReceive(mBit_Queue, (void *) &bitBericht[k], (TickType_t) 0) && k < 7)
//			// Vult 8 bits in een array om te veranderen naar een char
//				k++;
//
////			if(EOT == 1)
////			{
////				for(i=0; i<8 ; i++)
////					crcBuffer[i] = bitBericht[i];
////				EOT = 0;
////			}
////			else
////			{
//				for (i=0; i < 8 ; i++)
//				{
//					if(bitBericht[i] == 1)
//						letter += (pow(2, (7-i)));
//				}
//
//				LED_put(letter);
//
//				if(letter == 4)
//					EOT = 1;
//
//				if(letter != 0 && EOT == 0)
//					UART_putchar(letter);
//
//				letter = 0;
//			}
//		}
		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
//	       	sprintf(buf, "\r\n%s: %lu", __func__,uxQueueMessagesWaiting(mBit_Queue));
//			UART_puts(buf);
    	}
	}
}


