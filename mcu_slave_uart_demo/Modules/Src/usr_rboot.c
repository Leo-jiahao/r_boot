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
#include "usart.h"
#include "r_boot_slave.h"
#include "stm32f103rc_demo_r_boot_slave_flash.h"
#include <stdlib.h>


/**
 * @brief 
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
	if(addr == ENV_DEFAULT_CURRENT_APP_ADDR)
	{
		BSP_ErasePages(ENV_DEFAULT_CURRENT_APP_ADDR, ENV_DEFAULT_APP_MAX_SIZE/1024/2);
	}
    ret = BSP_WriteByteFlash((uint32_t)addr, (uint8_t *)src, len);

  }else if(addr >= ENV_DEFAULT_NEW_APP_ADDR 
  && addr + len < ENV_DEFAULT_NEW_APP_ADDR + ENV_DEFAULT_APP_MAX_SIZE){
	if(addr == ENV_DEFAULT_NEW_APP_ADDR)
	{
		BSP_ErasePages(ENV_DEFAULT_NEW_APP_ADDR, ENV_DEFAULT_APP_MAX_SIZE/1024/2);
	}
    ret = BSP_WriteByteFlash((uint32_t)addr, (uint8_t *)src, len);

  }else{
    ret = false;
  }

  return ret;
}
/**
 * @brief 
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
 * @brief 
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
 * @brief 
 * 
 * @param addr 
 * @param len 
 * @return true 
 * @return false 
 */
bool r_boots_output_stream(void *addr, int len)
{

	upacker_pack(&u_pack, addr, len);
	return true;
}
