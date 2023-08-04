/**
 * @file d_flash_config.h
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
#ifndef __D_FLASH_CONFIG_H
#define __D_FLASH_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f103rc_demo_r_boot_slave_flash.h"



/**
 * @brief 配置d flash模块的接口，以下接口缺一不可
 * 
 * 
*/

#define FLASH_START_ADDR_1B		        BSP_GetFlashAddr(14)
#define FLASH_START_ADDR_2B		        BSP_GetFlashAddr(18)

#define FLASH_SIZE_1B                   (4 * 2 * 1024)//8K
#define FLASH_SIZE_2B                   (4 * 2 * 1024)//8K
#define FLASH_ERASE_1B                  BSP_ErasePages(FLASH_START_ADDR_1B,4)//返回值bool类型
#define FLASH_ERASE_2B                  BSP_ErasePages(FLASH_START_ADDR_2B,4)//返回值bool类型

#define FLASH_READ_U32(address)         (*((volatile uint32_t *)(address)))
#define FLASH_WRITE(addr,src,len)       BSP_WriteFlash(addr,src,len)//返回值bool类型
#define USER_ITEM_MAX_NUM               (100)  //100条数据，数据类型不限，各自大小不限（总长度不能大于总空间）

#define ENABLE_D_FLASH                   1      //使能d flash
#define TRANSFER_THRESHOLD_PERCENT      (95)    //数据限制90%



#endif
