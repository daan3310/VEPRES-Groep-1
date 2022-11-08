/**
* @file 		data_rx.c
* @brief 		hier worden bits omgezet naar characters
*
* @author 		daniel roling
* @co-author 	Stein van Vliet
* @date 		17/10/2022
*/
#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"


/**
* @brief: 	Zet de ontvangen bits om in letters om te printen
* @param: 	argument, ongebruikt
* @return: 	void
*/
void Data_rx_task(void *argument)
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");

	int i;
	int length;

	uint8_t CrC;

	char letter;
	char charbuf[8];
	char bitbuf[64];

	while(TRUE)
	{
		//Reset alle ints/arrays
		i=0;
		length =0;
		memset(charbuf, '\0', 8);
		memset(bitbuf,'\0',64);

		osDelay(500);

		//Wacht op task notification
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		//Leeg de queue aan bytes
		while(xQueueReceive(mBit_Queue, (void *) &letter, (TickType_t) 0))
		{
			i++;
			//Als er nog geen 9 bytes zijn en de byte niet NULL is
			if(i<9)
			{
				if((Uart_debug_out & RX_DEBUG_OUT)&&letter!=0)
					UART_putchar(letter);
				s,10charbuf[i-1]=letter;
				length++;
			}
			//De 9de byte is de CRC, hier gebeurt de ontvangende check
			else
			{
//				UART_puts("\ntest");
//				UART_puts(charbuf);
				Char_to_bits(bitbuf,charbuf,length); 	//Maak van chararray een bitarray
//				UART_putint(length);
//				UART_puts(bitbuf);
				CrC = CRC_Builder(bitbuf,length*8);		//Calculeer CRC van bitarray

				//Debug
				if(Uart_debug_out & TEST_DEBUG_OUT)
				{
					if(CrC == letter)
						UART_puts("\nCRC checks out");
					else
						UART_puts("\nCRC Error!");
				}
			}
		}
	}
}
