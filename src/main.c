/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

// ****** Includes ******
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hwinit.h"

// ****** Define macros ******
#define TRUE 1
#define FALSE 0

#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

#define NOT_PRESSED FALSE
#define PRESSED TRUE

// ****** Global declarations ******
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;
TaskHandle_t xTaskHandle3 = NULL;
TaskHandle_t xTaskHandle4 = NULL;
QueueHandle_t xQueueHandle1 = NULL;
QueueHandle_t xQueueHandle2 = NULL;

char usr_msg[250];

uint8_t UART_ACCESS_KEY = AVAILABLE;

char Sys_Str[10];
uint8_t LedSt = 0;

// ****** Datatypes / Struct deginitions ******
typedef struct APP_CMD
{
	uint8_t COMMAND_NUM;
	uint8_t COMMAND_ARGS[10];
}APP_CMD_t;

// ****** Functions prototypes ******
void Task1_MenuDisplay_Handler(void *params);
void Task2_CmdHandling_Handler(void *params);
void Task3_CmdProc_Handler(void *params);
void Task4_UARTwrite_Handler(void *params);


#ifdef USE_SEMIHOSTING
	//Used for semihosting
	extern void initialise_monitor_handles();
#endif

// ****** Functions implementation ******

int main(void)
{

#ifdef USE_SEMIHOSTING
	initialise_monitor_handles();
	printf("This is Hello World example code \n");
#endif

	DWT->CTRL |= (1<<0); //Enable CYCCNT in DWT_CTRL

	//1. Resets the RCC clock config. to the default reset state.
	//Makes HSI On, PLL Off, HSE Off, system clock = 16MHz, cpu_clock = 16MHz.
	RCC_DeInit();

	//2. Update the SystemCoreClock var.
	SystemCoreClockUpdate();
	prvSetupHardware();

	// Start SEGGER Debugging
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	sprintf(usr_msg, "This is Queue app. starting \r\n");
	printmsg(usr_msg);

	//3. Create 4 tasks + 2 queue
	xQueueHandle1 = xQueueCreate(10, sizeof(APP_CMD_t*)); //Command queue
	xQueueHandle2 = xQueueCreate(10, sizeof(char*)); //UART msg queue
	if(xQueueHandle1 != NULL && xQueueHandle2 != NULL)
	{
		sprintf(usr_msg, "Queues created succesfully \r\n");
		printmsg(usr_msg);
		xTaskCreate(Task1_MenuDisplay_Handler,"MENU-DISPLAY-TASK",500,NULL,2,&xTaskHandle1);
		xTaskCreate(Task2_CmdHandling_Handler,"CMD-HANDLING-TASK",500,NULL,2,&xTaskHandle2);
		xTaskCreate(Task3_CmdProc_Handler,"CMD-PROC-TASK",500,NULL,2,&xTaskHandle3);
		xTaskCreate(Task4_UARTwrite_Handler,"UART-WRITE-TASK",500,NULL,2,&xTaskHandle4);
	}
	else
	{
		sprintf(usr_msg, "Queues creation failure \r\n");
		printmsg(usr_msg);
	}

	//4. Start the scheduler
	vTaskStartScheduler();

	for(;;);
}

// ***** Task Handlers *****
void Task1_MenuDisplay_Handler(void *params) // Task 1
{
	while(1)
	{

	}
}

void Task2_CmdHandling_Handler(void *params) // Task 2
{
	while(1)
	{

	}
}

void Task3_CmdProc_Handler(void *params) // Task 3
{
	while(1)
	{

	}
}

void Task4_UARTwrite_Handler(void *params) // Task 4
{
	while(1)
	{

	}
}


