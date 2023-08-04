/**
 * @file stream_packer.c
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

#include "stream_packer.h"
#include "crc.h"
#include "main.h"
static uint8_t frame_decode(upacker_inst_t packer, uint8_t d)
{
    if (packer->state == 0 && d == STX_H)
    {
        packer->state = 1;
        packer->calc = STX_H;
    }
    else if (packer->state == 1 && d == STX_L)
    {
        packer->calc += d;
        packer->state = 2;
    }
    else if (packer->state == 2)
    {
        //长度信息
        packer->flen = (uint16_t)d;
        packer->calc += d;

        packer->state = 3;
    }
    else if (packer->state == 3)
    {
        //长度信息
        packer->flen |= (uint16_t)d << 8;
        packer->calc += d;

        //数据包超长得情况下直接丢包
        if ((packer->flen) > MAX_PACK_SIZE)
        {
            packer->state = 0;
        }
        packer->state = 4;
        packer->cnt = 0;
    }
    else if (packer->state == 4)
    {
        //header校验
#if USE_DATA_CHECK
        if (d != (packer->calc & 0Xff))
        {
            packer->state = 0;
            return 0;
        }
#endif
        packer->state = 5;
    }

    else if (packer->state == 5)
    {
        packer->data[packer->cnt++] = d;
		if(packer->data[0] == 0xfe){
			packer->data[0] = 0;
		}
        if (packer->cnt == packer->flen)
        {
            packer->state = 6;
        }
    }
    else if (packer->state == 6)
    {
        packer->rx_crc16 = (uint16_t)d;
        packer->state = 7;
    }
    else if (packer->state == 7)
    {
        packer->rx_crc16 |= (uint16_t)d << 8;
        packer->state = 0;

#if USE_DATA_CHECK
        /* crc16计算 */
        packer->crc16 = crc16_calc(packer->data, packer->flen);
#else
        packer->crc16 = packer->rx_crc16;
#endif
        //接收完，检查check
        if ((packer->rx_crc16) == (packer->crc16))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        packer->state = 0;
    }
    return 0;
}

static uint8_t frame_encode(upacker_inst_t packer, uint8_t *data, uint16_t size)
{
    static uint8_t tmp[5] = {0};
    uint16_t crc16 = 0;
    static uint8_t crc8[2] = {0};
	static uint16_t args[3];
	static char *argv[3];
    if (size > 0xFFFF)
    {
        return 0;
    }

    /* 数据头 */
    tmp[0] = STX_H;
    tmp[1] = STX_L;

    /* 数据长度 */
    tmp[2] = size & 0xff;
    tmp[3] = (size >> 8) & 0xff;

    /* 头部校验 */
    tmp[4] = (tmp[0] + tmp[1] + tmp[2] + tmp[3]) & 0xff;

    /* crc16计算 */
    crc16 = crc16_2(data, size);
    crc8[0] = crc16 & 0xff;
    crc8[1] = (crc16 >> 8) & 0xff;
	args[0] = 5;
	args[1] = size;
	args[2] = 2;
	argv[0] = (char *)tmp;
	argv[1] = (char *)data;
	argv[2] = (char *)crc8;
	packer->send(3, args, argv);


    return 1;
}
/**
 * @brief  使用动态内存时需要初始化
 * @note   size pack缓存的长度，大于最大的数据包长度就行,使用PACK_SIZE
            无rtos最好用静态内存,不然要改heap
 * @param  *cmd_packer:
 * @param  *handler:
 * @retval None
 */
int upacker_init(upacker_inst_t packer, PACKER_CB h, PACKER_CB_S s)
{
#if USE_DYNAMIC_MEM
    packer->data = (uint8_t *)UP_MALLOC(MAX_PACK_SIZE);
    if (!packer->data)
    {
        return -1;
    }
#endif

    packer->cb = h;
    packer->send = s;
    return 0;
}

/**
 * @brief  解包输入数据
 * @note
 * @param  cmd_packer:
 * @param  *buff:
 * @param  size:
 * @retval None
 */
void upacker_unpack(upacker_inst_t packer, uint8_t *buff, uint16_t size)
{
    for (int i = 0; i < size; i++)
    {
        if (frame_decode(packer, buff[i]))
        {
            //解析成功,回调处理
            packer->cb(packer->data, packer->flen);

        }else{
			
		}
    }
}

/**
 * @brief  封包数据并发送
 * @note
 * @param  *packer:
 * @param  *buff:
 * @param  size:
 * @retval None
 */
void upacker_pack(upacker_inst_t packer, uint8_t *buff, uint16_t size) 
{
	frame_encode(packer, buff, size); 
}


