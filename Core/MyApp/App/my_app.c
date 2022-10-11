/**
* @file my_app.c
* @brief Bevat algemene functies, die normaal in main.c zouden staan.
* Echter, main.c wordt gegenereerd door de stm32-code generation tool,
* dus die hou ik nu zo leeg mogelijk.
*
* @author MSC
*
* @date 5/5/2022
*/
#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"

#include "my_app.h"

char *app_name    = "\r\n=== FreeRTOS_methods 1.0 ===\r\n";
char *app_nameLCD = "FRTS_methods 1.0"; // max 16 chars

/// default: debug all output to uart
int Uart_debug_out = DEBUG_OUT_NONE;


/**
* Schrijft applicatiedata (versie, werking, menu) naar UART.
* Les: je ziet hoe je handig een lange string kunt opstellen met het '\'-karakter.
* @return void
* @author MSC
*/
void DisplayVersion(void)
{
	char infobuf[100];
    osVersion_t osv;

    char *functionality =
"Deze applicatie laat de basis werking van verschillende FreeRTOS-mechanismen zien,\r\n\
zoals queueing, notifications, eventgroups, mutex-semaphores, hoe je makkelijk tasks\r\n\
creëert (tasks.c) én hoe je de bidirectionele omgang met de UART-comport gebruikt.\r\n\
Zie verder de Doxygen documentatie van de applicatie. \r\n\
Michiel Scager (update: july 2022)\r\n";

	LCD_clear();
	LCD_puts(app_nameLCD);

	UART_puts(app_name);
	UART_puts(functionality);

 	osKernelGetInfo(&osv, infobuf, sizeof(infobuf));
	UART_puts("\t-----");
	UART_puts("\r\n\t");                       UART_puts(infobuf);
	UART_puts("\r\n\tVersion: ");              UART_putint(osv.kernel);
	UART_puts("\r\n\tOS Timer freq (MHz): " ); UART_putint(osKernelGetSysTimerFreq()/1000000);
	UART_puts("\r\n\tOS Tick freq (Hz): " );   UART_putint(osKernelGetTickFreq());
	UART_puts("\r\n\t-----\r\n");

    DisplayMenu();
}


/**
* User Interface (menu) naar UART.
* @return void
* @author MSC
*/
void DisplayMenu(void)
{
	char *menu =
"\r\nMenu:=============================================================\r\n\
0: [on/off] ALL TEST OUTPUT\r\n\
1: [on/off] LEDS output\r\n\
2: [on/off] ARM_keys output\r\n\
3: [on/off] UART_keys output\r\n\
4: [on/off] STUDENT output\r\n\
5: [on/off] reserved output\r\n\
8: [toggle] Start Sync_Bytes\r\n\
9: [toggle] switch between 2200 Hz and 2800 Hz \r\n\
d: change Delay time (default 200), eg. 'd,50'\r\n\
p: change Priority of task, eg. 'p,7,20' sets priority of task 7 to 20\r\n\
t: display Task-data (number, priority, stack usage)\r\n\
b: change frequency (default 1 kHz), eg. 'b,2500'\r\n\
===================================================================\r\n";

    UART_puts(menu);
}


