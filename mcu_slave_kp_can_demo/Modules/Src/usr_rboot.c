/**
 * @file usr_rboot.c
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

#include "d_flash.h"
#include "usr_can.h"
#include "r_boot_slave.h"
#include "stm32f405_demo1_flash.h"
#include <stdlib.h>


/**
 * @brief 
 * 
 * @param app_size 
 * @return true 
 * @return false 
 */
static bool IAP_EraseAPPRunArea(uint32_t app_size)
{ 
  if(app_size <= 128 * 1024){
    if(BSP_EraseSector(FLASH_SECTOR_5)){
		  return true;
    }else{
      return false;
    }
  }else{
    if(BSP_EraseSector(FLASH_SECTOR_6) && BSP_EraseSector(FLASH_SECTOR_5)){
		  return true;
    }else{
      return false;
    }
  }
  
}
static bool IAP_EraseAPPStoreArea(uint32_t app_size)
{ 
  	
  if(app_size <= 128 * 1024){
    if(BSP_EraseSector(FLASH_SECTOR_7)){
		  return true;
    }else{
      return false;
    }
  }else if(app_size <= 256 * 1024){
    if(BSP_EraseSector(FLASH_SECTOR_7) && BSP_EraseSector(FLASH_SECTOR_8)){
		  return true;
    }else{
      return false;
    }
  }else{
    if(BSP_EraseSector(FLASH_SECTOR_7) && BSP_EraseSector(FLASH_SECTOR_8) && BSP_EraseSector(FLASH_SECTOR_9)){
		  return true;
    }else{
      return false;
    }
  }
}
/**
 * @brief r_boot接口
 * 
 * @param addr 
 * @param src 
 * @param len 
 * @return true 
 * @return false 
 */
bool r_boots_write_flash(uint32_t addr, char *src, int len)
{
  bool ret = false;
  static const char *env_key = "r_boot_env";
  uint32_t df_state;

  if(addr == FLASH_ENV_ADDRESS){

    if(df_write(env_key, src, len, &df_state) != DF_F_OK){
      ret = false;
    }else{
      ret = true;
    }

  }else if(addr >= ENV_DEFAULT_CURRENT_APP_ADDR 
  && addr + len < ENV_DEFAULT_CURRENT_APP_ADDR + ENV_DEFAULT_APP_MAX_SIZE){
	if(addr == ENV_DEFAULT_CURRENT_APP_ADDR){
		IAP_EraseAPPRunArea(129 * 1024);
	}
    ret = BSP_WriteByteFlash((uint32_t)addr, (uint8_t *)src, len);

  }else if(addr >= ENV_DEFAULT_NEW_APP_ADDR 
  && addr + len < ENV_DEFAULT_NEW_APP_ADDR + ENV_DEFAULT_APP_MAX_SIZE){
	if(addr == ENV_DEFAULT_NEW_APP_ADDR){
		IAP_EraseAPPStoreArea(129 * 1024);
	}
    ret = BSP_WriteByteFlash((uint32_t)addr, (uint8_t *)src, len);

  }else{
    ret = false;
  }

  return ret;
}
/**
 * @brief r_boot接口
 * 
 * @param addr 
 * @param dest 
 * @param len 
 * @return true 
 * @return false 
 */
bool r_boots_read_flash(uint32_t addr, char *dest, int len)
{
  bool ret = false;
  static const char *env_key = "r_boot_env";
  if(addr == FLASH_ENV_ADDRESS){

    if(df_read(env_key, dest, len) != DF_F_OK){
      ret = false;
    }else{
      ret = true;
    }

  }else if(addr >= ENV_DEFAULT_CURRENT_APP_ADDR 
  && addr + len < ENV_DEFAULT_CURRENT_APP_ADDR + ENV_DEFAULT_APP_MAX_SIZE){

    memcpy(dest, (const void *)addr, len);

    ret = true;
  }else if(addr >= ENV_DEFAULT_NEW_APP_ADDR 
  && addr + len < ENV_DEFAULT_NEW_APP_ADDR + ENV_DEFAULT_APP_MAX_SIZE){

    memcpy(dest, (const void *)addr, len);

    ret = true;
  }else{
    ret = false;
  }

  return ret;
}
/**
 * @brief r_boot接口
 * 
 * @param app_addr 
 * @return true 
 * @return false 
 */
bool r_boots_iap_jump(uint32_t app_addr)
{
  typedef void (*iap_function)(void);
  iap_function iap_fun = NULL;
  uint32_t iap_addr = NULL;

  if(( ( *(volatile uint32_t *)app_addr) & 0x2FFE0000U) == 0x20000000){
	  HAL_DeInit();
    iap_addr = *(volatile uint32_t *)(app_addr + 4);
    iap_fun = (iap_function)iap_addr;
    __set_MSP(*(volatile uint32_t *)iap_addr);
    iap_fun();
  }
  return false;
}
/**
 * @brief r_boot接口
 * 
 * @param addr 
 * @param len 
 * @return true 
 * @return false 
 */
bool r_boots_output_stream(void *addr, int len)
{
	char *tmpbuffer = (char *)malloc(len +4);
	*(uint32_t *)tmpbuffer = (len << 12 | 0);
	memcpy(&tmpbuffer[4], addr, len);
	
	KP_PutItem(kph, items[r_boot_slave_put], tmpbuffer, len+4, NULL);
    if(KP_WriteItem(kph, items[r_boot_slave_put],200,0) != KP_OK){
		free(tmpbuffer);
      return false;
    }else{
		free(tmpbuffer);
      return true;
    }
}



