################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hwinit.c \
../src/main.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/hwinit.o \
./src/main.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/hwinit.d \
./src/main.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DSTM32F407G_DISC1 -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/StdPeriph_Driver/inc" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Third-Party/SEGGER/SEGGER" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Third-Party/SEGGER/OS" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Third-Party/SEGGER/Config" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Config" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/Third-Party/FreeRTOS/org/Source/include" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/inc" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/CMSIS/device" -I"C:/Ac6/RTOS_Workspace/STM32_LedAndButton/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


