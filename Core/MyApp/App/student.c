/**
* @file student.c
* @brief Hier kunnen studenten hun eigen tasks aanmaken
*
* @author MSC

* @date 5/5/2022
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
void Student_task1 (void *argument)
{
	UART_puts((char *)__func__); UART_puts(" started\r\n");
	char buf[80];
	unsigned int i = 0;

	while(TRUE)
	{
       	osDelay(1000);

		if (Uart_debug_out & STUDENT_DEBUG_OUT)
		{
	       	sprintf(buf, "\r\n%s: %u", __func__, i++);
			UART_puts(buf);
    	}
	}
}

/**
 * @brief Veranderd de frequentie van timer 3
 * @param int De gewenste frequentie
 * @return void
 */
void Change_Frequency(int frequency)
{
	TIM3->ARR = 1000000 / frequency -1;
	TIM3->CCR3 = 1000000 / (frequency*2) -1;
}
