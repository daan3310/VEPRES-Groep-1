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
QueueHandle_t 	   hKey_Queue;
QueueHandle_t 	   hUART_Queue;
SemaphoreHandle_t  hLED_Sem;
EventGroupHandle_t hKEY_Event;
TimerHandle_t      hTimer1;


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

	BUZZER_put(1000);
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

	if (!(hKEY_Event = xEventGroupCreate()))
		error_HaltOS("Error hLCD_Event");

	if (!(hTimer1 = xTimerCreate("Timer_1", pdMS_TO_TICKS(TIMER1_DELAY), pdTRUE, 0, Timer1_Handler)))
		error_HaltOS("Error hTimer1");

	UART_puts("\n\rAll handles created successfully.");

	UART_puts("\n\rTimer set to: ");
	UART_putint((int)TIMER1_DELAY); // (int)-cast is nodig!
	xTimerStart(hTimer1, 0); // start de timer...
}


