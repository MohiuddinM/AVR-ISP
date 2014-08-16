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

#ifndef __ISP_H
#define	__ISP_H



/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported Defines ----------------------------------------------------------*/
#define ispTransmit			ispTransmit_hw
#define SPI_SPEED_HIGH		SPI_BAUDRATEPRESCALER_32
#define SPI_SPEED_NORMAL	SPI_BAUDRATEPRESCALER_64
#define SPI_SPEED_LOW		SPI_BAUDRATEPRESCALER_128
#define SPI_SPEED_LOWEST	SPI_BAUDRATEPRESCALER_256

#ifndef uchar
#define	uchar	unsigned char
#endif

/* Exported functions ------------------------------------------------------- */
int ispStart(unsigned char);
void ispWriteFuse(char, char);
void ispErase(void);
void ispConnect(); 							/* Prepare connection to target device */
void ispDisconnect();						/* Close connection to target device */
uchar ispTransmit_sw(uchar send_byte);		/* read an write a byte from isp using software (slow) */
uchar ispTransmit_hw(uchar send_byte);		/* read an write a byte from isp using hardware (fast) */
uchar ispEnterProgrammingMode();			/* enter programming mode */
uchar ispReadEEPROM(unsigned int address);	/* read byte from eeprom at given address */
uchar ispWriteFlash(unsigned long address,
		uchar data, uchar pollmode); 		/* write byte to flash at given address */
uchar ispFlushPage(unsigned long address, uchar pollvalue);
uchar ispReadFlash(unsigned long address); 	/* read byte from flash at given address */
uchar ispWriteEEPROM(unsigned int address,
		uchar data); 						/* write byte to eeprom at given address */
void ispSetSCKOption(uchar sckoption); 		/* set SCK speed. call before ispConnect! */
void ispLoadExtendedAddressByte(unsigned long address); /* load extended address byte */
/* Exported variables --------------------------------------------------------*/

#endif /* __ISP_H */
