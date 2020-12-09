/*
 * hwinit.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Matheus
 */

// ****** Includes ******
#include "hwinit.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// ***** Hardware initialization functions *****
void prvSetupHardware(void)
{
	prvSetupButton(); //KEY0 Init. from GPIO E
	prvSetupLED(); //LED0 Init. from GPIO F
	prvSetupUart(); //UART Init.
}

// Button related functions
void prvSetupButton(void)
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
void prvSetupLED(void)
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
void prvSetupUart(void)
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

	//4.1 Enable UART byte - reception interrupt
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	NVIC_SetPriority(USART2_IRQn,5);
	NVIC_EnableIRQ(USART2_IRQn);

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
