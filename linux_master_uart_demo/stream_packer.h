/**
 * @file stream_packer.h
 * @author Leo-jiahao (leo884823525@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-01
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

#ifndef _DRV_PACKER_H_
#define _DRV_PACKER_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

//使用动态内存
#define USE_DYNAMIC_MEM 0

#if USE_DYNAMIC_MEM
#define UP_MALLOC
#define UP_FREE
#endif

#define USE_DATA_CHECK 0

#define MAX_PACK_SIZE 2048 //最长消息长度,最大可用16位

#define STX_L 0X5A         //数据头
#define STX_H 0XA5         //数据头

    typedef void (*PACKER_CB)(uint8_t *d, uint16_t s);

    typedef struct
    {
#if !USE_DYNAMIC_MEM
        uint8_t data[MAX_PACK_SIZE]; //payload的内存
#else
    uint8_t *data; //用来做payload序列化的内存
#endif
        uint16_t flen;     //frame长度

        uint16_t rx_crc16; //获取到的CRC值
        uint16_t crc16;    //计算到的CRC值

        uint8_t calc;      //frame校验计算值
        uint8_t state;     //frame解析状态
        uint16_t cnt;      //frame数据接收cnt

        PACKER_CB cb;      //数据包处理回调
        PACKER_CB send;    //数据发送回调

    } upacker_inst;

    typedef upacker_inst *upacker_inst_t;

    int upacker_init(upacker_inst_t packer, PACKER_CB handle, PACKER_CB send);
    void upacker_pack(upacker_inst_t packer, uint8_t *buff, uint16_t size);
    void upacker_unpack(upacker_inst_t packer, uint8_t *buff, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif
