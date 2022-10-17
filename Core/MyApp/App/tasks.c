/**
* @file tasks.c
* @brief Bevat de functies die tasks aanmaakt én hulpfuncties voor tasks.<br>
* <b>Demonstreert: osThreadNew(), vTaskSuspendAll(), xTaskResumeAll(), uxTaskGetStackHighWaterMark(),
* vTaskPrioritySet() en c-stuff als array-handling, structures,</b> en let op:
* het gebruiken van specifieke structure-members binnen andere structures, zie tasks[]!<br>
*
* Elke task wordt op dezelfde manier aangemaakt, met osThreadNew() die een trits
* aan argumenten wil, maw: de argumenten zijn de variabelen. Dus waarom niet per task
* de variabelen op een rijtje zetten in een array - zo krijg je veel meer overzicht.
* Eerst wordt een structure aangemaakt, die de argumenten gaat bevatten, struct TaskData.
*
* TaskData bevat 4 members: func (de taskname), argument (eventueel argument voor task, nu niet gebruikt),
* struct attr (FreeRTOS structure, waarvan we enkele gebruiken!), de taskhandle.
* In de array tasks[] worden alleen die members van TaskData geinitialiseerd die nodig zijn om
* elke task/thread aan te maken. <br>Dat zijn: 1. de task, 2. het argument voor de task, 3. de tasknaam
* als string, 4. de stack size, 5. de prioriteit.
* De members 3, 4 en 5 zijn members van de attr structure.
*
* @author MSC
* @date 5/5/2022
*/

#include "main.h"
#include "cmsis_os.h"
#include "my_app.h"

/// struct voor taak-gegevens, waaronder de argumenten om een taak aan te maken
typedef struct TaskData
{
	osThreadFunc_t       func;     /// de naam van de taak zelf
	void *               argument; /// evt extra variabele voor taak, bv. "hello"
	osThreadAttr_t       attr;     /// struct for OS, but we only declare a few members
	osThreadId_t         hTask;    /// taskhandle, aangemaakt door osThreadNew()
} TASKDATA, *PTASKDATA;			   //  typedefs of struct and pointer to it


/** tasks[] is een array van structures met alleen de argumenten om een taak aan te maken.
 *
 * - **NB:**<br>
 * 	- Dit wist je vast niet. De members .attr.name en .attr.stack_size etc. zijn enkele members van de struct osThreadAttr_t;
 * enkele daarvan zijn nodig om een taak te starten en dus alleen die heb ik in deze array geinitialiseerd - dat kan
 * dus op de manier die je hier ziet.
 * 	- Als je in de tweede kolom ipv NULL "hallo" zet, wordt "hallo" aan de taak meegegeven - kun je evt gebruiken voor
 * testen of iets dergelijks.
 * 	- De prioriteiten worden hier met default-waarden gezet; hier kun je zelf mee spelen. Via de UART kun je elke
 * taak een andere prioriteit geven. Kun je ook mee spelen. Trouwens, als je taken dezelfde prioriteit
 * geeft, dan gaat het OS ze met Round Robin keurig allemaal gelijk verdelen.
 * 	- De laatste regel bevat nullen; deze regel wordt gebruikt om for-loepjes eenvoudig te laten eindigen.
 * 	- Om een taak niet te laten starten, zet gewoon '//' voor de regel en Bob is je oom!
 *
 * - **Doel van de array:**
 * 	-# makkelijk en overzichtelijk om taken te starten
 * 	-# geeft een overview van alle taken en argumenten (name, stacksize, prty's)
 */
TASKDATA tasks[] =
{
// function      arg   name                        stacksize (* 4 = 32bit)     priority
// ----------------------------------------------------------------------------------------------------------------------------
	// in ARM_keys.c
{ ARM_keys_IRQ, NULL, .attr.name = "ARM_keys_IRQ", .attr.stack_size = 128 * 6, .attr.priority = osPriorityNormal1 },
{ ARM_keys_task,NULL, .attr.name = "ARM_keys_task",.attr.stack_size = 128 * 6, .attr.priority = osPriorityNormal2 },

	// UART_keys.c
{ UART_keys_IRQ,NULL, .attr.name = "UART_keys_IRQ",.attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal5 },
{ UART_menu,    NULL, .attr.name = "UART_menu",    .attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal6 },

	// student.c
{ Student_task1,NULL, .attr.name = "Student_task1",.attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal7 },
  // DataRx.c
//{ DataRx,		NULL, .attr.name = "DataRx",	   .attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal6 },


	// data_tx.c
{ Prep_data_task,NULL, .attr.name = "Prep_data_task",.attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal7 },
{ Send_data_task,NULL, .attr.name = "Send_data_task",.attr.stack_size = 128 * 6, .attr.priority = osPriorityBelowNormal7 },

  // deze laatste niet wissen, wordt gebruik als 'terminator' in loops
{ NULL,         NULL, .attr.name = NULL,         .  attr.stack_size = 0,       .attr.priority = 0 }
};


/**
* @brief Creëert alle taken door te scannen door de TASKDATA-argumenten-array.<br>
* Let op. Eerst wordt de scheduler gestopt, dan de taken gecreëerd, dan de scheduler gestopt.
* Waarom? Anders gaan taken gelijk beginnen en wachten op taken die nog niet bestaan, dat geeft een rommeltje.
* @return void
*/
void CreateTasks(void)
{
	PTASKDATA ptd = tasks; // tasks == &tasks[0]: both are addresses of first struct
	vTaskSuspendAll(); // zet de scheduler stil en dwing FreeRTOS om eerst alle taken te starten

	for (; ptd->func != NULL; ptd++) // loopje zonder teller; gewoon de pointer steeds ophogen...
	{
		UART_puts("\r\nCreated: ");
		UART_puts((char *)ptd->attr.name);

		// nb. kijk naar de haakjes van argument 3: het gaat om het adres van (structure attr).
		ptd->hTask = osThreadNew(ptd->func, ptd->argument, &(ptd->attr));
	}

	xTaskResumeAll();   // start nu de scheduler: play ball
	DisplayTaskData();  // display alle taskdata op UART
}


/**
* @brief Zoekt in tasks-array de task, en returns de handle.
* Deze handle wordt door CreateTasks() in de array geplaatst. In plaats van deze functie kan ook
* de OS-functie xTaskGetHandle() gebruikt worden. Alleen is 'mijn' functie sneller.
* @param *taskname De tasknaam
* @return osThreadId_t De taskhandle
*/
osThreadId_t GetTaskhandle(char *taskname)
{
	PTASKDATA ptd = tasks;

	for (; ptd->func != NULL; ptd++)
		if (!strcmp(taskname, ptd->attr.name))
			return (ptd->hTask);

	return (NULL); // if task was not found...
}


/**
* @brief Verandert de prioriteit van een taak.
* Eerst wordt de taak gezocht in de lijst van alle taken (tasks[])
* Daarna wordt de priority in de lijst gewijzigd en prioriteit van de taak aangepast. De functie
* DisplayTaskData() toont op de UART het tasknummer dat je nodig hebt.
* @param task_id Task identifier
* @param new_priority Nieuwe prioriteit
* @return void
*/
void SetTaskPriority(int task_id, int new_priority)
{
	PTASKDATA ptd = tasks;
	unsigned int 	task_nr;


	for (task_nr = 1; ptd->func != NULL; ptd++, task_nr++)
		if (task_nr == task_id) // hebbes
		{
			ptd->attr.priority = new_priority;			// struct-member
			vTaskPrioritySet(ptd->hTask, new_priority); // taskpriority
			// of
			// osThreadSetPriority(ptd->hTask, new_priority);

			UART_puts("\r\nSet task "); UART_puts(ptd->attr.name);
			UART_puts("'s priority to: "); UART_putint(new_priority);
			return;
		}
}


/**
* @brief Displays de stack-gegevens van alle taken op de UART
* @return void
*/
void DisplayTaskData(void)
{
	PTASKDATA ptd = tasks;
	UBaseType_t freespace;
	unsigned int used;
	unsigned int task_nr; // tasknr for changing priority

	for (task_nr=1; ptd->func != NULL; ptd++, task_nr++)
	{
		freespace = uxTaskGetStackHighWaterMark(ptd->hTask) * 4; 	// amount of free bytes
		used = 100 - (freespace * 100) / ptd->attr.stack_size; 		// in percentage

		UART_puts("\r\n\t["); UART_putint(task_nr); UART_puts("] ");
		UART_puts(ptd->attr.name);
		UART_puts("\t priority: "); UART_putint(ptd->attr.priority);
		UART_puts("\t stacksize: "); UART_putint(ptd->attr.stack_size);
		UART_puts("\t used: "); UART_putint(used); UART_puts("%");
	}
}


