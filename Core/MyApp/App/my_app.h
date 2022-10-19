/**
* @file my_app.h
* @brief Bevat de defines & externals voor deze applicatie
* @attention
* <h3>&copy; Copyright (c) 2021 (HU) Michiel Scager.</h3>
* @author MSC
*
* @date 5/5/2022
*/
#include "freeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "task.h" // taskYield
#include "semphr.h"
#include "math.h"
#include "structs.h"

#include "stdio.h"
#include "string.h" // strtok, strcpy
#include "stdlib.h" // atoi
#include "ctype.h"  // toupper
#include "math.h" // power

#define TRUE  	   1
#define FALSE      0

/// set queue op 10 elementen
#define QSIZE_UART 64
/// set software timer 500 msecs
#define TIMER1_DELAY 500
/// set data q max length
#define QSIZE_DATA 64

/// alle handles
/// handle voor UART-queue
extern QueueHandle_t 	  	hUART_Queue;
/// handle voor data queue
extern QueueHandle_t 		hChar_Queue;
extern QueueHandle_t 		hBit_Queue;
extern QueueHandle_t 		mBit_Queue;
/// handle voor LED-mutex
extern SemaphoreHandle_t 	hLED_Sem;
/// handle voor ARM-keys-event
extern EventGroupHandle_t 	hKEY_Event;
/// handle voor speaker timer
extern TIM_HandleTypeDef 	htim3;

extern TimerHandle_t hSample_Timer;



/// debug naar uart output, zie uart_keys.c
/// bitmask-toggle key voor task-debug-output
extern int Uart_debug_out;
/// stop alle test output
#define DEBUG_OUT_NONE 		0x00
/// genereer alle test output
#define DEBUG_OUT_ALL 		0xff

/// bit 1: toggles leds stuff output
#define LEDS_DEBUG_OUT   	0x01
/// bit 2: toggles armkeys stuff output
#define ARMKEYS_DEBUG_OUT 	0x02
/// bit 3: toggles uart stuff output
#define UART_DEBUG_OUT    	0x04
/// bit 4: toggles student stuff output
#define STUDENT_DEBUG_OUT  	0x08
/// bit 5: toggles reserved1 output
#define RES1_DEBUG_OUT 		0x10
/// bit 6: toggles reserved2 output
#define RES2_DEBUG_OUT 	    0x24

/// Redefine pins om beter aan te geven waar het om gaat: gekleurde ledjes
/// LD4_Pin
#define LEDGREEN  GPIO_PIN_12
/// LD3_Pin
#define LEDRED    GPIO_PIN_14
/// LD5_Pin
#define LEDORANGE GPIO_PIN_13
/// LD6_Pin
#define LEDBLUE   GPIO_PIN_15

// my_app.c
extern void DisplayVersion (void);
extern void DisplayMenu    (void);
extern void error_HaltOS   (char *);

// tasks.c
extern void         DisplayTaskData (void);
extern void         CreateTasks     (void);
extern osThreadId_t GetTaskhandle   (char *);
extern void         SetTaskPriority (int, int);

// handles.c
extern void         CreateHandles   (void);

// ARM_keys.c
extern void ARM_keys_IRQ (void *);
extern void ARM_keys_task(void *);

// UART_keys.c
extern void UART_keys_poll(void *);
extern void UART_keys_IRQ (void *);
extern void UART_menu     (void *);

// student.c
extern void Student_task1 (void *);
// data_rx.c
extern void DataRx1 ();
// microfoon.c
extern void Sample_Handler(TimerHandle_t);

// speaker.c
#define SAMPLERATE 20
#define FREQHIGH 2800
#define FREQLOW 2200

extern void Speaker_Init();
extern void Change_Frequency(int);
extern void Toggle_Frequency();
extern void Sync_Bytes();
extern void Disable_Speaker();

//data_tx.c
extern void Prep_data_task();
extern void Send_data_task();
extern void Char_to_bits(char*, char*, int);

