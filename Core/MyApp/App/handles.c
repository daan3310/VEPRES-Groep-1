/**
* @file handles.c
* @brief Definieert en creëert alle handles (semaphores, notifications, queues, eventgroups etc.).<br>
* <b>Demonstreert: xSemaphoreCreateMutex(), xQueueCreate(), xEventGroupCreate(),
* xTimerCreate(), xTimerStart().</b>
* Mocht er wat misgaan bij creëren van een handle, dan wordt het OS op 'stop' gezet, zie error_HaltOS().<br>
*
* <b>WAARSCHUWING:</b><br>
* Dit gaat vaak fout. Het WEL definiëren en gebruiken van handles, maar VERGETEN ze te <u>creëren!</u>
* De compiler geeft dan geen foutmelding (de variabele bestaat immers, heb je aangemaakt), maar staat nog
* op NULL, want zonder ...Create() heb je het OS niet aan het werk gezet om de administratie
* voor die handle aan te maken.
**
* @author MSC
* @date 5/5/2022
*/

#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

// all handles, note: defined to 'extern' in my_app.h
QueueHandle_t 	   	hKey_Queue;
QueueHandle_t 	   	hUART_Queue;
QueueHandle_t		hChar_Queue;
QueueHandle_t		hBit_Queue;
SemaphoreHandle_t  	hLED_Sem;
EventGroupHandle_t 	hKEY_Event;
TimerHandle_t      	hTimer1;


/**
* @brief Fatale OS-error opgetreden tijdens startup of runtime, doorgaan is
* zinloos.
* Het OS wordt tot stoppen gebracht, er wordt output gegenereerd waar mogelijk,
* dus op UART en LCD.
* @param msg Foutmelding
* @return void
*/
void error_HaltOS(char *msg)
{
	LCD_puts(msg);
	UART_puts(msg); UART_puts(". Application halted\r\n");

	vTaskSuspendAll(); // stop alle tasks

	while (TRUE)
	{
	}
}


/**
* @brief Creates alle handles voor deze applicatie
* @return void
*/
void CreateHandles(void)
{
	if (!(hLED_Sem = xSemaphoreCreateMutex()))
		error_HaltOS("Error hLED_Sem");

	if (!(hUART_Queue = xQueueCreate(QSIZE_UART, sizeof(unsigned int))))
		error_HaltOS("Error hUART_Q");

	if(!(hChar_Queue = xQueueCreate(QSIZE_DATA, sizeof(char))))
		error_HaltOS("Error hChar_Queue");

	if(!(hBit_Queue = xQueueCreate(QSIZE_DATA*8, sizeof(char))))
			error_HaltOS("Error hBit_Queue");

	if (!(hKEY_Event = xEventGroupCreate()))
		error_HaltOS("Error hLCD_Event");

	UART_puts("\n\rAll handles created successfully.");
}


