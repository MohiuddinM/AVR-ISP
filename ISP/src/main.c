/*
    STM32 based AVR programmer
    Copyright (C) 2014  Muhammad Mohiuddin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "diag/Trace.h"
#include "isp.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN USBD_HandleTypeDef hUSBDDevice __ALIGN_END;

unsigned int pageFlashAddr = 0,
			 pageSize,
			 totalWritten,
			 cmd = 0,
			 temp,
			 bytesWritten = 0;


/* Private function prototypes -----------------------------------------------*/
void SysClockConfig(void);
int SPI_command(void);
void Init(void);
void verify(void);
void fuseProgram(void);
/*----------------------------------------------------------------------------*/


/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{

	// GPA4 -> RST
	// GPA5 -> SCK
	// GPA6 -> MISO
	// GPA7 -> MOSI

	Init();
	while (ComPortClosed);
	HAL_Delay(500);

	if(ispStart(3) == 0) printf("0\n");
	else printf("0\n");
	while(cmd != END)
	{
		scanf("%u", &cmd);
		switch(cmd)
		{
		case CMD_PROG_FLASH:
			printf("%d\n", SPI_command());
			break;
		case CMD_DEF_PAGE_SIZE:
			scanf("%u", &pageSize);
			printf("0\n", pageSize);
			break;
		case CMD_SET_FUSE:
			fuseProgram();
			break;
		case CMD_START_ISP:
			scanf("%u", &temp);
			if(ispStart(temp) == 0) printf("0\n");
			else printf("1\n");
			break;
		case CMD_FLASH_ERASE:
			ispErase();
			HAL_Delay(100);
			printf("0\n");
			break;
		case CMD_VERIFY_FLASH:
			verify();
			break;
		case CMD_READ_FLASH:
			break;
		default:
			ispDisconnect();
			printf("0\n");
			break;
		}

	}
	while(1)
	{
		BSP_LED_Toggle(LED4);
		HAL_Delay(40);
	}
}
int SPI_command(void)
{
	unsigned int  address;
	unsigned int dataByte;
	address = 0;
	while(1)
	{
		scanf("%u", &dataByte);
		if(dataByte > 255)
		{
			ispFlushPage(address - bytesWritten, 0);
			HAL_Delay(6);
			bytesWritten = 0;
			return 0;
		}
		ispWriteFlash(address, dataByte, 0);
		address++;
		bytesWritten++;
		totalWritten++;
		if(bytesWritten == pageSize)
		{
			ispFlushPage(address - pageSize, 0);
			HAL_Delay(6);
			bytesWritten = 0;
		}
	}
	return 0;
}
void fuseProgram(void)
{
	unsigned char hFuse, lFuse;
	scanf("%hhu", &hFuse);
	scanf("%hhu", &lFuse);
	ispWriteFuse(hFuse, lFuse);
	HAL_Delay(10);
	printf("CMD_SET_FUSE - OK\n");
}
void verify(void)
{
	for(unsigned int i = 0; i < totalWritten; i++)
	{
		printf("%u\n", ispReadFlash(i));
	}
	printf("END\n");
}
void Init(void)
{
	SystemInit();

	// Enable HSE Oscillator and activate PLL with HSE as source
	SysClockConfig();
	SystemCoreClockUpdate();

	// Initialize the HAL Library; it must be the first
	// instruction to be executed in the main program.
	HAL_Init();

	/* Init USB Device Library */
	USBD_Init(&hUSBDDevice, &VCP_Desc, 1);

	/* Add Supported Class */
	USBD_RegisterClass(&hUSBDDevice, &USBD_CDC);

	/* Add CDC Interface Class */
	USBD_CDC_RegisterInterface(&hUSBDDevice, &USBD_CDC_fops);

	/* Start Device Process */
	USBD_Start(&hUSBDDevice);

	/* Configure LED3, LED4, LED5, LED6 and PB*/
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);


	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOA_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

}
void Error_Handler(void)
{
	/* Turn LED5 (RED) on */
	BSP_LED_On(LED5);
	printf("Error !!!");
	while (1)
	{
	}
}
void SysClockConfig(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	// Enable Power Control clock
	__PWR_CLK_ENABLE();

	// The voltage scaling allows optimizing the power consumption when the
	// device is clocked below the maximum system frequency, to update the
	// voltage scaling value regarding system frequency refer to product
	// datasheet.
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	// Enable HSE Oscillator and activate PLL with HSE as source
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	// This assumes the HSE_VALUE is a multiple of 1MHz. If this is not
	// your case, you have to recompute these PLL constants.
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	// Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	// clocks dividers
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
