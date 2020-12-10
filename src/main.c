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

uint8_t command_buffer[20];
uint8_t command_len = 0;

char userfdbck[]={"\
\r\nGotten! "};

char menu[]={"\
\r\nLED ON                ---> 1 \
\r\nLED OFF               ---> 2 \
\r\nLED TOGGLE            ---> 3 \
\r\nLED TOGGLE OFF        ---> 4 \
\r\nLED READ STATUS       ---> 5 \
\r\nRTC PRINT DATETIME    ---> 6 \
\r\nEXIT APP              ---> 7 \
\r\nType below:"};

char usr_msg[250];

uint8_t UART_ACCESS_KEY = AVAILABLE;

char Sys_Str[10];
uint8_t LedSt = 0;

// ****** Datatypes / Struct deginitions ******
#define LED_ON_COMMAND 1
#define LED_OFF_COMMAND 2
#define LED_TOGGLE_ON 3
#define LED_TOGGLE_OFF 4
#define LED_READ_STATUS 5
#define RTC_PRINT 6
#define EXIT_APP 7

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
uint8_t getCommandCode(uint8_t *buffer);
void getArguments(uint8_t *buffer);


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

	sprintf(usr_msg, "This is Queue Multitasking app. starting \r\n");
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
	char *pData = menu;
	while(1)
	{
		xQueueSend(xQueueHandle2,&pData,portMAX_DELAY); //Relying on queue's size
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}
}

void Task2_CmdHandling_Handler(void *params) // Task 2
{
	uint8_t command_code = 0;
	APP_CMD_t *new_cmd;

	while(1)
	{
		//Send Cmd to queue
		command_code = getCommandCode(command_buffer); // Critical sector, global data access
		new_cmd = (APP_CMD_t*) pvPortMalloc(sizeof(APP_CMD_t));
		new_cmd->COMMAND_NUM = command_code;
		getArguments(new_cmd->COMMAND_ARGS);
		xQueueSend(xQueueHandle1,&new_cmd,portMAX_DELAY);
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}
}


void Task3_CmdProc_Handler(void *params) // Task 3
{
	APP_CMD_t *new_cmd;

	while(1)
	{
			xQueueReceive(xQueueHandle1,(void*)&new_cmd,portMAX_DELAY);
			if(new_cmd->COMMAND_NUM == LED_ON_COMMAND)
			{
				GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
			}
			else if(new_cmd->COMMAND_NUM == LED_OFF_COMMAND)
			{
				GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
			}
			else if(new_cmd->COMMAND_NUM == LED_TOGGLE_ON)
			{
			}
			else if(new_cmd->COMMAND_NUM == LED_TOGGLE_OFF)
			{
			}
			else if(new_cmd->COMMAND_NUM == LED_READ_STATUS)
			{
			}
			else if(new_cmd->COMMAND_NUM == RTC_PRINT)
			{
			}
			else if(new_cmd->COMMAND_NUM == EXIT_APP)
			{
			}
	}
}

void Task4_UARTwrite_Handler(void *params) // Task 4
{
	char *pData = NULL;
	while(1)
	{
		xQueueReceive(xQueueHandle2,&pData,portMAX_DELAY);
		printmsg(pData);
	}
}

void USART2_IRQHandler()
{
	uint16_t data_byte;
	//char *pData = userfdbck;
	BaseType_t *xHigherPriTask = pdTRUE;

	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)) //data received from user
	{
		data_byte = USART_ReceiveData(USART2);
		command_buffer[command_len++]= data_byte & 0xFF;
		if(data_byte == 13) //finished entering data
		{
			command_len=0;
			//xQueueSend(xQueueHandle2,&pData,portMAX_DELAY); //Send user feedback
			xTaskNotifyFromISR(xTaskHandle2,0,eNoAction,NULL); 	//notify command handling task
			xTaskNotifyFromISR(xTaskHandle1,0,eNoAction,NULL); 	//notify menu display task

		}
	}

}

uint8_t getCommandCode(uint8_t *buffer)
{
	return buffer[0]-48;
}

void getArguments(uint8_t *buffer)
{

}
