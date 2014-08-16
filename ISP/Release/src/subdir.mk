################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/isp.c \
../src/main.c \
../src/stm32f4xx_it.c \
../src/usbd_cdc_interface.c \
../src/usbd_conf.c \
../src/usbd_desc.c 

OBJS += \
./src/isp.o \
./src/main.o \
./src/stm32f4xx_it.o \
./src/usbd_cdc_interface.o \
./src/usbd_conf.o \
./src/usbd_desc.o 

C_DEPS += \
./src/isp.d \
./src/main.d \
./src/stm32f4xx_it.d \
./src/usbd_cdc_interface.d \
./src/usbd_conf.d \
./src/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DOS_USE_TRACE_ITM -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/CMSIS" -I"../system/include/stm32f4-hal" -I"C:\Users\INSPIRON\workspace\VCP\system\STM32_USB_Device_Library\Class\CDC\Inc" -I"C:\Users\INSPIRON\workspace\VCP\system\STM32_USB_Device_Library\Core\Inc" -I"C:\Users\INSPIRON\workspace\VCP\include" -I"C:\Users\INSPIRON\workspace\VCP\system\BSP\STM32F4-Discovery" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


