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

// ****** Define macros ******
#define TRUE 1
#define FALSE 0

#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

#define NOT_PRESSED FALSE
#define PRESSED TRUE

// ****** Global declarations ******
int SysTick_Counter_Interruptions = 0;
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;
char usr_msg[250];
uint8_t UART_ACCESS_KEY = AVAILABLE;
uint8_t button_status_flag = NOT_PRESSED;
char Sys_Str[10];
uint8_t LedSt = 0;

// ****** Functions prototypes ******
void LED_TASK_handler(void *params);
void BUTTON_TASK_handler(void *params);

static void prvSetupHardware(void);
static void prvSetupButton(void);
static void prvSetupLED(void);
static void prvSetupUart(void);
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

// ***** Hardware initialization functions *****
static void prvSetupHardware(void)
{
	prvSetupButton(); //KEY0 Init. from GPIO E
	prvSetupLED(); //LED0 Init. from GPIO F
	prvSetupUart(); //UART Init.
}

// Button related functions
static void prvSetupButton(void)
{
	//RCC AHB1 clock init.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	//Define the GPIO struct for configuration
	GPIO_InitTypeDef gpio_key0_pins;

	//Configure the GPIO behavior
	gpio_key0_pins.GPIO_Mode = GPIO_Mode_IN;
	gpio_key0_pins.GPIO_Pin = GPIO_Pin_4;
	gpio_key0_pins.GPIO_OType = GPIO_OType_PP;
	gpio_key0_pins.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_key0_pins.GPIO_Speed = GPIO_Low_Speed;

	//Initiate the GPIO
	GPIO_Init(GPIOE,&gpio_key0_pins);
}

// LED Related functions
static void prvSetupLED(void)
{
	//RCC AHB1 clock init.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//Define the GPIO struct for configuration
	GPIO_InitTypeDef gpio_led0_pins;

	//Configure the GPIO behavior
	gpio_led0_pins.GPIO_Mode = GPIO_Mode_OUT;
	gpio_led0_pins.GPIO_Pin = GPIO_Pin_9;
	gpio_led0_pins.GPIO_OType = GPIO_OType_PP;
	gpio_led0_pins.GPIO_PuPd = GPIO_PuPd_DOWN;
	gpio_led0_pins.GPIO_Speed = GPIO_Speed_100MHz;

	//Initiate the GPIO
	GPIO_Init(GPIOF,&gpio_led0_pins);
}

// UART Related functions
static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	//1. Enable UART periph. clock.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	// PA2 -> TX and PA3 -> RX

	//2. Alternate function configuration of MCU pins to behave as UART2 TX and RX.
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins)); //Clean garbage from variables (zeroing)
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	//3. AF Mode settings for the pins
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	//4. UART parameter initialization
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&uart2_init);

	//5. Enable UART Periph.
	USART_Cmd(USART2,ENABLE);
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
