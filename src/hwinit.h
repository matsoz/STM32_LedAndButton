/*
 * hwinit.h
 *
 *  Created on: Nov 30, 2020
 *      Author: Matheus
 */

#ifndef HWINIT_H_
#define HWINIT_H_
#endif /* HWINIT_H_ */

// ****** Define macros ******
#define TRUE 1
#define FALSE 0

#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

#define NOT_PRESSED FALSE
#define PRESSED TRUE


// ****** Includes ******
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

// ****** Prototypes ******
void prvSetupHardware(void);
void prvSetupButton(void);
void prvSetupLED(void);
void prvSetupUart(void);
