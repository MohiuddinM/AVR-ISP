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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CMD_PROG_FLASH		1
#define CMD_DEF_PAGE_SIZE	2
#define CMD_SET_FUSE		3
#define CMD_START_ISP		4
#define CMD_FLASH_ERASE		5
#define CMD_VERIFY_FLASH	6
#define CMD_READ_FLASH		7
#define CMD_PROG_EEPROM		8
#define CMD_READ_EEPROM		9
#define END					255
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
/* Exported variables --------------------------------------------------------*/
unsigned char spiSpeed;

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
