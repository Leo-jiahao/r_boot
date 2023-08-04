/**
 * @file stm32f103rc_demo_r_boot_slave_flash.c
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
#include "stm32f103rc_demo_r_boot_slave_flash.h"
#include "string.h"
/**
 * @brief 获取地址所在的page
 * 
 * @param address 
 * @return uint32_t 
 */
uint32_t BSP_GetPageIndex(uint32_t address)
{
    return (address-FLASH_BASE) / FLASH_PAGE_SIZE;
}

uint32_t BSP_GetFlashAddr(uint32_t page_index)
{

    return FLASH_BASE + FLASH_PAGE_SIZE * page_index;

}
/**
 * @brief 擦除该扇区
 * 
 * @param sector 
 * @return bool 
 */
bool BSP_ErasePages(uint32_t start_page_addr, uint32_t page_num)
{
    __set_PRIMASK(1);//关中断
	HAL_FLASH_Unlock();
	
	uint32_t SectorError;
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;       	//擦除类型，扇区擦除 
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.PageAddress = start_page_addr;
    EraseInitStruct.NbPages = page_num;

	if(HAL_FLASHEx_Erase(&EraseInitStruct,&SectorError) != HAL_OK)
	{
		goto FLASH_ERRR;
	}

		
	HAL_FLASH_Lock();    
	__set_PRIMASK(0);
	return true;	
	
FLASH_ERRR:
	HAL_FLASH_Lock();    
	__set_PRIMASK(0);
	return false;
}
/**
 * @brief 写字流
 * 
 * @param dest 目的地址，4字节对齐
 * @param src 源地址
 * @param len 字长
 * @return bool 
 */
bool BSP_WriteFlash(uint32_t dest, uint32_t *src, uint32_t len)
{
	HAL_StatusTypeDef FlashStatus;
    __set_PRIMASK(1);//关中断
	HAL_FLASH_Unlock();
	

	for(int i = 0,j = 0; i < len; i++,j++)
	{
		FlashStatus = FLASH_WaitForLastOperation(FLASH_WAIT_TIME);	//等待flash上次操作完成
		if(FlashStatus != HAL_OK)
		{
			goto FLASH_ERRR;
		}
        
		FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(dest + j * 4),*((uint32_t *)(&src[i])));
		if(FlashStatus != HAL_OK)
		{
			goto FLASH_ERRR;
		}
	}	
	
	HAL_FLASH_Lock();    
	__set_PRIMASK(0);
	return true;	
	
FLASH_ERRR:
	HAL_FLASH_Lock();    
	__set_PRIMASK(0);
	return false;
}
/**
 * @brief 按字节写入，不足4字节的部分，补齐写入
 * 
 * @param dest 
 * @param src 
 * @param len 
 * @return true 
 * @return false 
 */
bool BSP_WriteByteFlash(uint32_t dest, void *src, uint32_t len)
{
	uint16_t word_num = len >> 2;
	uint16_t byte_remain = len & (0x0003U);
	uint32_t tmp = 0xffffffffu;
	uint32_t *p_u32 = src;

	if(BSP_WriteFlash(dest, p_u32, word_num) != true){
		return false;
	}
	
	if(byte_remain){
		memcpy(&tmp, (void *)((uint32_t)src + word_num * 4), byte_remain);
		
		if(BSP_WriteFlash(dest + word_num * 4, &tmp, 1) != true){
			return false;
		}
	}
	return true;
}

