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
#include "hwinit.h"

// ****** Global declarations ******
int SysTick_Counter_Interruptions = 0;
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;
char usr_msg[250];
char Sys_Str[10];
uint8_t UART_ACCESS_KEY = AVAILABLE;
uint8_t button_status_flag = NOT_PRESSED;
uint8_t LedSt = 0;

// ****** Functions prototypes ******
void LED_TASK_handler(void *params);
void BUTTON_TASK_handler(void *params);


void printmsg(char *msg);
void rtos_delay(uint32_t delay_in_ms);

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

	DWT->CTRL |= (1<<0); //Enable CYCCNT in DWT_CTRL (Segger SisView)

	//1. Resets the RCC clock config. to the default reset state.
	//Makes HSI On, PLL Off, HSE Off, system clock = 16MHz, cpu_clock = 16MHz.
	RCC_DeInit();

	//2. Update the SystemCoreClock var.
	SystemCoreClockUpdate();
	prvSetupHardware();

	// Start SEGGER Debugging
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	sprintf(usr_msg, "This is Hello World app. starting \r\n");
	printmsg(usr_msg);

	//3. Create 2 tasks
	xTaskCreate(LED_TASK_handler,"LED-TASK",500,NULL,2,&xTaskHandle1);
	xTaskCreate(BUTTON_TASK_handler,"BUTTON-TASK",500,NULL,2,&xTaskHandle2);

	//4. Start the scheduler
	vTaskStartScheduler();

	for(;;);
}

// ***** Task Handlers *****
void LED_TASK_handler(void *params) //LED management task
{
	uint32_t button_notification_val = 0;
	while(1)
	{
		if(xTaskNotifyWait(0x0,0x0,&button_notification_val,0xFFFFFFFF) == pdTRUE) //Task should wait until notification is received
		{
			GPIO_ToggleBits(GPIOF,GPIO_Pin_9);
			sprintf(usr_msg,"\n\r ------->>> Notification is received - %ld",button_notification_val);
			printmsg(usr_msg);
		}
	}
}

void BUTTON_TASK_handler(void *params) //Button pooling task
{
	while(1)
	{
		//If button Key0 is pressed, the pin is grounded, so equals ZERO
		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
		{
			rtos_delay(1000);
			printmsg("\n\r --->> Button pressed ");
			xTaskNotify(xTaskHandle1,0x1,eIncrement);
		}
	}
}


void printmsg(char *msg)
{
	for(uint32_t i=0; i<strlen(msg) ; i++)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET) ;
		USART_SendData(USART2, msg[i]);
	}

}

void rtos_delay(uint32_t delay_in_ms)
{
	uint32_t current_tick_count = xTaskGetTickCount();
	uint32_t delay_in_ticks = configTICK_RATE_HZ * (delay_in_ms/1000);
	while(xTaskGetTickCount() < (current_tick_count + delay_in_ticks));
}
