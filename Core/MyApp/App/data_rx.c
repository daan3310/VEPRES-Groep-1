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
void Data_rx_task()
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char letter;
	while(TRUE)
	{
		osDelay(500); //Tom: Ik weet niet waarom maar deze moet relatief hoog zijn
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		while(xQueueReceive(mBit_Queue, (void *) &letter, (TickType_t) 0))
			if(letter != 0)
				UART_putchar(letter);

	}
}

//		if (Uart_debug_out & STUDENT_DEBUG_OUT)
//		{
//	       	sprintf(buf, "\r\n%s: %lu", __func__,uxQueueMessagesWaiting(mBit_Queue));
//			UART_puts(buf);
//    	}
