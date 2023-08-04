/**
 * @file stm32f103rc_demo_r_boot_slave_flash.h
 * @author Leo-jiahao (leo884823525@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright 
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) <2023>  <Leo-jiahao> 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */
#ifndef __STM32F103RC_DEMO_R_BOOT_SLAVE_FLASH_H__
#define __STM32F103RC_DEMO_R_BOOT_SLAVE_FLASH_H__

#include <stdbool.h>

#include "stm32f1xx_hal.h"
#define FLASH_WAIT_TIME 1000

#define PAGE_SIZE       FLASH_PAGE_SIZE
#define PAGE_NUMBER     128

uint32_t BSP_GetPageIndex(uint32_t address);

uint32_t BSP_GetFlashAddr(uint32_t page_index);

bool BSP_ErasePages(uint32_t start_page_addr, uint32_t page_num);

bool BSP_WriteFlash(uint32_t dest, uint32_t *src, uint32_t len);

bool BSP_WriteByteFlash(uint32_t dest, void *src, uint32_t len);
#endif

