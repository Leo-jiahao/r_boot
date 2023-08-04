/**
 * @file stream_packer.h
 * @brief 链路层通讯协议
 *
 * @author dalin (dalin@open-robot.com)
 * @version 1.0
 * @date 2023-06-12
 *
 * @copyright Copyright (c) 2023  Buffalo Robot Tech.co, Ltd
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2023-06-12 <td>1.0     <td>yangsl     <td>Init
 * </table>
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
	typedef void (*PACKER_CB_S)(int  argc, uint16_t *args, char **argv);

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
        PACKER_CB_S send;    //数据发送回调

    } upacker_inst;

    typedef upacker_inst *upacker_inst_t;

    int upacker_init(upacker_inst_t packer, PACKER_CB handle, PACKER_CB_S send);
    void upacker_pack(upacker_inst_t packer, uint8_t *buff, uint16_t size);
    void upacker_unpack(upacker_inst_t packer, uint8_t *buff, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif
