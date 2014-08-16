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
#include "main.h"
#include "isp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define spiHWdisable()		SPIx_Disable()
#define spiHWenable()		SPIx_Enable()

#define RESET_LOW()			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define RESET_HIGH()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#define SCK_LOW()			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define SCK_HIGH()			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)

#define ispDelay(x)			HAL_Delay(x)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uchar isp_hiaddr;

unsigned char speeds[4] = {SPI_SPEED_HIGH, SPI_SPEED_NORMAL, SPI_SPEED_LOW, SPI_SPEED_LOWEST};
/* Private functions ---------------------------------------------------------*/
void ispWriteFuseLow(char);
void ispWriteFuseHigh(char);

/*----------------------------------------------------------------------------*/

int ispStart(unsigned char speed)
{
	spiSpeed = speeds[speed];
	ispConnect();
	SPIx_Init();

	for(int i = 0; i < 5; i++)
	{
		if(ispEnterProgrammingMode() == 0)
		{
			return 0;
		}
	}
	Error_Handler();
	return 1;
}

void ispConnect()
{

	/* all ISP pins are inputs before */
	/* now set output pins */
//
	/* reset device */
	RESET_LOW(); /* RST low */
	SCK_LOW(); /* SCK low */

	/* positive reset pulse > 2 SCK (target) */
	ispDelay(1);
	RESET_HIGH(); /* RST high */
	ispDelay(1);
	RESET_LOW(); /* RST low */

	spiHWenable();

	/* Initial extended address value */
	isp_hiaddr = 0;
}

void ispDisconnect()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//RESET_HIGH();

	/* disable hardware SPI */
	//spiHWdisable();
}

uchar ispTransmit_hw(uchar send_byte)
{
	return SPIx_WriteRead(send_byte);
}

uchar ispEnterProgrammingMode()
{
	uchar check;
	uchar count = 32;

	while (count--)
	{
		ispTransmit(0xAC);
		ispTransmit(0x53);
		check = ispTransmit(0);
		ispTransmit(0);

		if (check == 0x53)
		{
			return 0;
		}

		spiHWdisable();

		/* pulse RST */
		ispDelay(1);
		RESET_HIGH(); /* RST high */
		ispDelay(1);
		RESET_LOW(); /* RST low */
		ispDelay(1);
		spiHWenable();
	}

	return 1; /* error: device dosn't answer */
}

static void ispUpdateExtended(unsigned long address)
{
	uchar curr_hiaddr;

	curr_hiaddr = (address >> 17);

	/* check if extended address byte is changed */
	if (isp_hiaddr != curr_hiaddr)
	{
		isp_hiaddr = curr_hiaddr;
		/* Load Extended Address byte */
		ispTransmit(0x4D);
		ispTransmit(0x00);
		ispTransmit(isp_hiaddr);
		ispTransmit(0x00);
	}
}

uchar ispReadFlash(unsigned long address)
{

	ispUpdateExtended(address);

	ispTransmit(0x20 | ((address & 1) << 3));
	ispTransmit(address >> 9);
	ispTransmit(address >> 1);
	return ispTransmit(0);
}

uchar ispWriteFlash(unsigned long address, uchar data, uchar pollmode)
{
	ispUpdateExtended(address);

	ispTransmit(0x40 | ((address & 1) << 3));
	ispTransmit(address >> 9);
	ispTransmit(address >> 1);
	ispTransmit(data);

	if(pollmode == 0) return 0;
	else ispDelay(1);
	return 0;
}

void ispErase(void)
{
	ispTransmit(0xAC);
	ispTransmit(0x80);
	ispTransmit(0);
	ispTransmit(0);
	ispDelay(10);
	int i = 4;
	while (--i)
	{
		RESET_HIGH();
		ispDelay(1);
		RESET_LOW();
		ispDelay(1);
	}
}
void ispWriteFuse(char high, char low)
{
	ispWriteFuseLow(low);
	ispWriteFuseHigh(high);
}
void ispWriteFuseLow(char fuse)
{
	ispTransmit(0xAC);
	ispTransmit(0xA0);
	ispTransmit(0);
	ispTransmit(fuse);
	ispDelay(5);
}
void ispWriteFuseHigh(char fuse)
{
	ispTransmit(0xAC);
	ispTransmit(0xA8);
	ispTransmit(0);
	ispTransmit(fuse);
	ispDelay(5);
}
uchar ispFlushPage(unsigned long address, uchar pollvalue)
{
	ispUpdateExtended(address);

	ispTransmit(0x4C);
	ispTransmit(address >> 9);
	ispTransmit(address >> 1);
	ispTransmit(0);

	if(pollvalue == 0) return 0;
	else ispDelay(1);
	return 0;
}

uchar ispReadEEPROM(unsigned int address)
{
	ispTransmit(0xA0);
	ispTransmit(address >> 8);
	ispTransmit(address);
	return ispTransmit(0);
}

uchar ispWriteEEPROM(unsigned int address, uchar data)
{

	ispTransmit(0xC0);
	ispTransmit(address >> 8);
	ispTransmit(address);
	ispTransmit(data);

	ispDelay(10);

	return 0;
}
