/**
* @file UART_keys.c
* @brief Behandelt de communicatie met de UART.<br>
* <b>Demonstreert: IRQ-handling, queue-handling, tasknotifcation, string-handling (strtok, pointers).</b><br>
* Aan de UART is een interrupt gekoppeld, waarvan de ISR (zie: HAL_UART_RxCpltCallback())
* in main.c gegenereerd is. Elke toets (character), die in een terminalprogramma ingedrukt wordt, wordt in de ISR
* in een queue gezet.<br>
* De task UART_keys_IRQ() leest de queue uit nadat het LINEFEED-character is gevonden (nadat ENTER gedrukt is). De
* task leest de queue uit en vult een eigen buffer. Deze buffer wordt via TaskNotify doorgestuurd naar de task die
* deze buffer interpreteert: UART_menu().
* @author MSC
*
* @date 5/5/2022
*/

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"
#include "uart.h"

extern unsigned int os_delay; /// deze waarde kan hier veranderd worden.
/**
* @brief Polt en leest characters in die de gebruiker via Terminalprogramma intikt.
* 1. leest characters van uart
* 2. notifies command-string naar andere task
* @param *argument Niet gebruikt, eventueel een waarde of string om te testen
* @return void
*/
void UART_keys_poll (void *argument)
{
    char buffer[QSIZE_UART];

	osThreadId_t hTask = xTaskGetHandle("UART_menu");

	UART_puts((char *)__func__); UART_puts(" started, enter key on terminal\n\r");

	while(TRUE)
    {
	    UART_gets(buffer, QSIZE_UART, TRUE); // wait for string

    	xTaskNotify(hTask, (int) buffer, eSetValueWithOverwrite); // notify task2 with value

		if (Uart_debug_out & UART_DEBUG_OUT)
		{
			UART_puts("\n\r"); UART_puts((char *)__func__);
			UART_puts("- notified with "); UART_puts(buffer);
			UART_puts("\r\n");
		}
	}
}


/**
* @brief Leest characters in die de gebruiker via Terminalprogramma intikt.
* 1. wacht op chars in queue, gevuld door de IRQ_handler (main.c)
* 2. notifies/stuurt command-string naar andere task
* @param *argument Niet gebruikt, eventueel een waarde of string om te testen
* @return void
*/
void UART_keys_IRQ (void *argument)
{
    char  		    buffer[QSIZE_UART];
    char		    buffer_copy[QSIZE_UART];
	int 			pos = 0;
	int             finish = FALSE;
	osThreadId_t    hTask;

	UART_puts((char *)__func__); UART_puts("started, enter key on terminal...\r\n");

	if (!(hTask = xTaskGetHandle("UART_menu")))
		error_HaltOS("Err:UART_hndl 0");

	while(TRUE)
    {
		// laat de VIC de q vullen, tot LFEED gedrukt is of qsize - 1 bereikt is.
		// nb: argument 2 is het adres waar ik het q-char in zet
		// nb: q-receive haalt gelijk de buffer leeg (q-peek niet).
		xQueueReceive(hUART_Queue, &buffer[pos], portMAX_DELAY);

		// negeer dit char bij geen data: -1, 255, of CR of spatie
		if (buffer[pos] == -1 || buffer[pos] == 255 || buffer[pos] == CRETURN)
			continue;

//		UART_putchar(buffer[pos]);  // echo

		// check of de string gesloten was of gesloten moet worden...
		if (buffer[pos] == LFEED)     // close als LF gedrukt
		{
			buffer[pos] = '\0';
			finish = TRUE;

		}
		else if (pos == QSIZE_UART - 2) // close if end of buf
		{
			buffer[++pos] = '\0';       // first, skip to last position, then close string
			finish = TRUE;
		}

		if(finish)
		{
			// de volgende taak krijgt een copy van de string
			// mijn eigen buffer kan zo gelijk weer gevuld worden door de ISR
			strcpy(buffer_copy, buffer);
			memset(buffer, 0, QSIZE_UART); // clear original buffer
			finish = FALSE;
			pos = 0;

			xTaskNotify(hTask, (int) buffer_copy, eSetValueWithOverwrite); // notify task2 with copy

			if (Uart_debug_out & UART_DEBUG_OUT)
			{
				UART_puts("\n\r"); UART_puts((char *)__func__);
				UART_puts(": UART_menu notified with "); UART_puts(buffer_copy);
				UART_puts("\r\n");
			}
		}
		else
			pos++;
	}
}



/**
* @brief User Interface. De task wacht op kant en klare user-strings (TaskNotifyTake) van ISR-handler, en interpreteert
* de string. Het eerste character geeft het hoofd-commando aan. Afhankelijk van het commando worden opvolgende chars
* geinterpreteerd.
* @param *argument Niet gebruikt, eventueel een waarde of string om te testen
* @return void
*/
void UART_menu (void *argument)
{
	char 	arr[8];
	char   *s;
	char   *tok = ",";  // token if command is more than 1 char
	int     val1, val2;
	int 	input;
	int 	i;
	osThreadId_t    hTask;

	UART_puts((char *)__func__); UART_puts("started\n\r");
  
	if (!(hTask = xTaskGetHandle("Prep_data_task")))
			error_HaltOS("Error UART_menu: Prep_data_task handle");

	while (TRUE)
	{
		// wacht op de string; let op de cast van (unsigned long) naar (char *)!
		// want de waarde die ik terug krijg is een pointer.
		s = (char *)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// check het eerste (en vaak enige) karakter van de inkomende command-string
		// toupper() zorgt ervoor dat het eerste karakter hoofdletter wordt, zo hoef ik niet op kleine
		// letters te checken.
		switch (toupper((unsigned char)s[0]))
		{

			default:
				UART_puts(s);
				UART_puts(" (unkown command)\r\n");
				break;

			/// <b>0 - 5</b>: Togglet verschillende debug-outputs naar UART
			case '0':
				Uart_debug_out = (Uart_debug_out ? DEBUG_OUT_NONE : DEBUG_OUT_ALL);
				UART_puts("\r\nall debug output = ");
				UART_puts(Uart_debug_out == DEBUG_OUT_ALL ? "ON\r\n" : "OFF\r\n");

				// als alle output uitgezet wordt, is het handig om gelijk het menu te laten zien.
				if (Uart_debug_out == DEBUG_OUT_NONE)
					DisplayMenu();
				break;

			case '1':
				Uart_debug_out ^= LEDS_DEBUG_OUT; // toggle output on/off
				UART_puts("\r\nleds output = ");
				UART_puts(Uart_debug_out & LEDS_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '2':
				Uart_debug_out ^= ARMKEYS_DEBUG_OUT; // toggle output on/off
				UART_puts("\r\narmkeys output = ");
				UART_puts(Uart_debug_out & ARMKEYS_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '3':
				Uart_debug_out ^= UART_DEBUG_OUT; // toggle output on/off
				UART_puts("\r\nuart output = ");
				UART_puts(Uart_debug_out & UART_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '4':
				Uart_debug_out ^= TEST_DEBUG_OUT; // toggle output on/off
				UART_puts("\r\ntest output = ");
				UART_puts(Uart_debug_out & TEST_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '5':
				Uart_debug_out ^= SEND_DEBUG_OUT;
				UART_puts("\r\nsend output = ");
				UART_puts(Uart_debug_out & SEND_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '6':
				Uart_debug_out ^= SAMPLE_DEBUG_OUT;
				UART_puts("\r\nsample output = ");
				UART_puts(Uart_debug_out & SAMPLE_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '7':
				Uart_debug_out ^= TX_DEBUG_OUT;
				UART_puts("\r\ntx output = ");
				UART_puts(Uart_debug_out & TX_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '8':
				Uart_debug_out ^= RX_DEBUG_OUT;
				UART_puts("\r\nrx output = ");
				UART_puts(Uart_debug_out & RX_DEBUG_OUT ? "ON\r\n" : "OFF\r\n");
				break;

			case '9':
				Toggle_Speaker(START);
				Toggle_Frequency();
				break;

			case 'C':		//CRC test via de terminal
				input = atoi(s+2);

				UART_puts("\r\nCRC-check voor:");
				UART_putint(input);
				UART_puts("\r\nResult: ");

				for(i=0;i<8;i++)
				{
					arr[i] = (input>>(7-i)) & 1; //Stop byte in bitarray
					UART_putint(arr[i]);
				}

				CRC_Builder(arr,8);	//Geef bitarray aan CRC functie
				break;

			case 'S':		//Verander de sample- en zendsnelheid via de terminal
				input = atoi(s+2); // skip first 2 characters

				UART_puts("\r\nSpeed changed to: ");
				UART_putint(input); UART_puts(" ms");

				Speed_calc(input);	//Geef nieuwe snelheid mee aan de snelheidscalculatie
				break;

			case 'M':
				DisplayMenu(); /// M: Displays het menu (zie my_app.c)
				break;

			case 'D':
				DisplayTaskData(); /// T: Displays de stackdata van alle Tasks
				break;

			case 'T':
				s += 2;  // skip de ,

				// print naar lcd
				LCD_clear();
				LCD_put("Transmit");
				LCD_XY(0,1);
				LCD_put(s);

				// print naar terminal
				UART_puts("\r\nTransmitting: ");
				UART_puts(s);

				// blijf data in de Q stoppen zolang *s niet NULL is
				while(*s != 0)
				{
					xQueueSend(hChar_Queue, s, 0);
					s++;
				}
				// geef het stokje door aan Prep_data_task
				xTaskNotifyGive(hTask);
				break;


			case 'P':
				/// P: Verandert de Proriteit van een taak
				/// commando, als: <b>"t,9,20"</b> betekent: set Task 9 op priority 20
				//  eerst: de 2 waarden worden uit de string gehaald met strtok()
				//  dan: de strings worden naar int geconverteerd
				//  nb. dit is wel grof geprogrammeerd zo, in het echt maak je hier een mooie functie van.
				s = strtok(s,    tok); 				 // naar start van string, negeer 't,'

				s = strtok(NULL, tok); val1 = atoi(s); // volgende = task_id
				s = strtok(NULL, tok); val2 = atoi(s); // volgende = priority

				if (val1 && val2)						 // kleine validiteitscontrole
					SetTaskPriority(val1, val2);

				break;

			case 'B':
				/// P: Veranderd de output frequentie van de buzzer
				/// commando: <b>"b,200"</b> betekent: set frequentie op 200, NB: spaties worden niet afgevangen...
				input = atoi(s+2); // skip first 2 characters
				UART_puts("\r\n Frequency set to: ");
				UART_putint(input);
				Change_Frequency(input);
				Toggle_Speaker(START);
				break;

			case 'L':
				/// L: zet het display uit
				LCD_display_off();
				break;

			case 'O':
				/// O: zet het display aan
				LCD_display_on();
				LCD_cursor_off();
				break;
		}
	}
}
