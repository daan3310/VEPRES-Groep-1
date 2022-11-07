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
	int i;
	int length;
	uint8_t CrC;
	char charbuf[8];
	char bitbuf[64];
	while(TRUE)
	{
		i=0;
		memset(charbuf, 0, 8);
		memset(bitbuf,'\0',64);
		osDelay(500); //Tom: Ik weet niet waarom maar deze moet relatief hoog zijn
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		while(xQueueReceive(mBit_Queue, (void *) &letter, (TickType_t) 0))
		{
			i++;
			if(i<9)
			{
				if(letter!=0)
				{
					UART_putchar(letter);
					charbuf[i-1]=letter;
					length++;
				}
			}
			else
			{
				Char_to_bits(bitbuf,charbuf,length);
				CrC = CRC_Builder(bitbuf,length*8);
				if(CrC == letter)
					UART_puts("\nCRC checks out");
				else
					UART_puts("\nCRC Error!");
//				UART_puts("\n");
//				UART_putint(letter);
			}
		}
	}
}

//		if (Uart_debug_out & STUDENT_DEBUG_OUT)
//		{
//	       	sprintf(buf, "\r\n%s: %lu", __func__,uxQueueMessagesWaiting(mBit_Queue));
//			UART_puts(buf);
//    	}
