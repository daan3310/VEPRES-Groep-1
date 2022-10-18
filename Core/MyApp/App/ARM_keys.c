/**
* @file ARM_keys.c
* @brief Behandelt de communicatie met de ARM-toetsjes met: Eventgroups, TaskNotify, Interrupt-handling.<br>
* <b>Demonstreert: xEventGroupWaitBits(), xTaskGetHandle(), xTaskNotify(), xTaskNotifyWait(),xSemaphoregive(), xSemaphoreTake(). </b><br>
*
* Aan de ARM-keys is een interrupt gekoppeld (zie stm32f4xx_it.c). Die stuurt een event
* door die opgevangen wordt door task ARM_keys_IRQ().
* @author MSC
*
* @date 5/5/2022
*/

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"


/**
* @brief Zet een kleurenledje aan en uit.
* @param color De kleur.
* @return void.
*/
void toggle_led (uint32_t color)
{
	HAL_GPIO_TogglePin(GPIOD, color);   // turns led on or off
	osDelay(20);
	HAL_GPIO_TogglePin(GPIOD, color);   // turns led on or off
}


/**
* @brief Deze task handelt de ARM-toets af, die ontvangen is van de ISR-handler (zie: stm32f4xx_it.c).
* @param *argument Niet gebruikt, eventueel een waarde of string om te testen
* @return void.
*/
void ARM_keys_IRQ (void *argument)
{
	unsigned int key;
	osThreadId_t hTask;

	UART_puts("\r\n"); UART_puts((char *)__func__); UART_puts(" started");

	if (!(hTask = xTaskGetHandle("ARM_keys_task")))
		error_HaltOS("Err:ARM_hndle");

    while (1)
	{
		// wait for ISR (EXTI0_IRQHandler()) to signal that a key is pressed
		key = xEventGroupWaitBits(hKEY_Event, 0xffff, pdTRUE, pdFALSE, portMAX_DELAY );

		xTaskNotify(hTask, key, eSetValueWithOverwrite); // notify task2 with value
	}
}


/**
* @brief Task krijgt ARM-key met notificatie binnen, en zet ledjes op die waarde.
* Ook de gekleurde ledjes (behalve blauw, die wordt door de timer gebruikt) krijgen
* een schwung...
* @param *argument Niet gebruikt, eventueel een waarde of string om te testen.
* @return void.
*/
void ARM_keys_task (void *argument)
{
	uint32_t 	 key;
	char stringBuf[30];
	osThreadId_t    hTask;

	if (!(hTask = xTaskGetHandle("Prep_data_task")))
	error_HaltOS("Error UART_menu: Send_data_task handle");

	while(TRUE)
	{
		// WAITING FOR users key
        xTaskNotifyWait (0x00,      		// Don't clear any notification bits on entry.
        		 	 	 0xffffffff, 		// ULONG_MAX, reset the notification value to 0 on exit.
    	                 &key, 				// Notified value.
    	                 portMAX_DELAY);  	// Block indefinitely.

    	LED_put((unsigned char)key); // set 8 leds-byte to key-value
		osDelay(500);


		switch(key)
		{
		case 1:
			sprintf(stringBuf, "Button 1");
			break;

		case 2:
			sprintf(stringBuf, "Button 2");
			break;

		case 3:
			sprintf(stringBuf, "Button 3");
			break;

		case 4:
			sprintf(stringBuf, "Button 4");
			break;

		case 5:
			sprintf(stringBuf, "Button 5");
			break;

		case 6:
			sprintf(stringBuf, "Button 6");
			break;

		case 7:
			sprintf(stringBuf, "Button 7");
			break;

		case 8:
			sprintf(stringBuf, "Button 8");
			break;

		case 9:
			sprintf(stringBuf, "Button 9");
			break;

		case 10:
			sprintf(stringBuf, "Button 10");
			break;

		case 11:
			sprintf(stringBuf, "Button 11");
			break;

		case 12:
			sprintf(stringBuf, "Button 12");
			break;

		case 13:
			sprintf(stringBuf, "Button 13");
			break;

		case 14:
			sprintf(stringBuf, "Button 14");
			break;

		case 15:
			sprintf(stringBuf, "Button 15");
			break;

		case 16:
			sprintf(stringBuf, "Button 16");
			break;

		default:
			sprintf(stringBuf, "Default");
			break;
		}
		char *s = stringBuf;
		while(*s != 0)	//puts string to queue for tx
		{
			xQueueSend(hChar_Queue, s, 0);
			s++;
		}

		if (Uart_debug_out & ARMKEYS_DEBUG_OUT)
		{
			UART_puts("\r\n\tARM_key pressed: "); UART_putint(key);
		}
		LCD_clear();
		LCD_put("Transmit");
		LCD_put(stringBuf);
		xTaskNotifyGive(hTask);	//gives task to tx

     	taskYIELD(); // done, force context switch
	}
}

