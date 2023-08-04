/**
 * @file main.c
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


#include "dev_can.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "r_boot_master.h"
#include "kpcan.h"



int rd=0;
char *ftp_buffer = NULL;
CAN_PORT can0 = NULL;
KP_handle my_can;



extern int rd;




/**
 * @brief 
 * 
 * @param buffer 
 * @param buffer_size 
 */
void u_r_bootm_log(char * buffer, int buffer_size)
{
    for (int i = 0; i < buffer_size; i++)
    {
        putchar(buffer[i]);
        
        /* code */
    }
}


/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
static void *u_r_boot_m_timer_Thread(void *arg)
{
    while(1){
        
        usleep(1000);

        r_bootm_timer_handler(1);
        
    }
}
/**
 * @brief 
 * 
 */
static void u_ftp_ok_callback(void)
{
    printf("transmit test.bin ok\n");
    // free(ftp_buffer);
} 
/**
 * @brief 
 * 
 */
static void u_ftp_error_callback(void)
{
    printf("transmit test.bin error\n");
    // free(ftp_buffer);
}
typedef struct
{
    uint8_t m_id;
    uint8_t s_id;
    KP_Data_TypeDef type;
    //对于块传输成功后的第一个32bit，代表当前块号，块末尾有4字节:buff_size到buff_size+4，表示当前块有效字节长度（包含块号和数据）、和crc16校验数据
    uint16_t buff_size;
}ITEM_TypeDef;

typedef enum
{
    r_boot_slave_get = 0,

    r_boot_slave_put,
	
    item_num,
}DEV_ITEMS_TypeDef;

const ITEM_TypeDef items_attri[item_num] = {
    [r_boot_slave_get]              = { .m_id = 1, .s_id = 0 , .type = KP_DATA_TYPE_BLOCK     , .buff_size = 2048 },
    
    [r_boot_slave_put]             = { .m_id = 2, .s_id = 1 , .type = KP_DATA_TYPE_BLOCK   , .buff_size = 2048 },

};

item_t items[item_num] = {NULL, };
void u_sendFrameDef(KP_canframe msg);
/**
 * @brief 
 * 
 * @param port 
 */
void * can0ReceiveThread(void * u_port)
{
    CAN_PORT port = u_port;
    int res;
    struct can_frame frame;
    KP_State_TypeDef state;
    KP_CANFrame_TypeDef rcv, reply;
    void* blockaddr;
    uint16_t blocksize;
    while (port->used) {

       res = CAN_RECV(port->fd, &frame, sizeof(frame), MSG_WAITALL);
        if (res < 0)
        {
            perror("CAN_RECV err");
            continue;
        }
        printf("CAN_RECV frame_id:0X%4X, dlc:%d ", frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++)
        {
            printf("0X%2X ",frame.data[i]);
            /* code */
        }
        printf("\n");
        
        rcv.can_id = frame.can_id & CAN_EFF_MASK;
        rcv.dlc = frame.can_dlc;

        memcpy (rcv.data, frame.data, rcv.dlc);   

        state = KP_Despatch(my_can, &rcv, &reply);
        if( state == KP_BT_Succ &&
            items_attri[r_boot_slave_put].m_id == rcv.data[1] &&
            items_attri[r_boot_slave_put].s_id == rcv.data[2]){
            //获取块数据地址 获取块注册时的大小
            u_sendFrameDef(&reply);
            blockaddr =  KP_GetItemAddr(my_can, items[r_boot_slave_put], &blocksize); 

            r_bootm_despatch(rd, (void *)( ( uint32_t ) blockaddr + 4),
            KP_BLOCK_VALID_BSIZE(blockaddr, blocksize));

            // for(int i = 0; i < KP_BLOCK_VALID_BSIZE(blockaddr, blocksize); i++){
            //     putchar(( *(char *)(( uint32_t ) blockaddr + 4 + i)));

            // }

        }
        if(state == KP_OK  || state == KP_BT_Fail)
        {
            u_sendFrameDef(&reply);
        }
    }
}

/**
 * @brief 
 * 
 * @param buffer 
 * @param buffer_size 
 * @return true 
 * @return false 
 */
bool u_r_bootm_output_stream(char * buffer, int buffer_size)
{
    char *tmpbuffer = (char *)malloc(buffer_size + 4);
    *(uint32_t *)tmpbuffer = (buffer_size<<12 | 0);
    memcpy(&tmpbuffer[4], buffer, buffer_size);
    KP_PutItem(my_can, items[r_boot_slave_get], tmpbuffer, buffer_size+4, NULL);

    if(KP_WriteItem(my_can, items[r_boot_slave_get], 0, 0) == KP_OK){
        free(tmpbuffer);
        return true;
    }
    free(tmpbuffer);
    return false;

}
pthread_mutex_t r_boot_mutex;

void EnterMutex(void)
{
	if(pthread_mutex_lock(&r_boot_mutex)) {
		fprintf(stderr, "pthread_mutex_lock() failed\n");
	}
}

void LeaveMutex(void)
{
	if(pthread_mutex_unlock(&r_boot_mutex)) {
		fprintf(stderr, "pthread_mutex_unlock() failed\n");
	}
}
/**
 * @brief 
 * 
 * @param ms 
 */
void u_delay(uint32_t ms)
{
    usleep(1000*ms);
}
/**
 * @brief 
 * 
 * @param msg 
 */
void u_sendFrameDef(KP_canframe msg)
{
    Message m;
    m.cob_id = msg->can_id;
    m.len = msg->dlc;
    memcpy(m.data, msg->data, m.len);
    m.rtr = 0;
    canSend(can0, &m);
}
/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    int ret = 0;

    my_can = KP_Init(1,"master_node", KP_MASTER, malloc, free, u_delay, u_sendFrameDef);
    for (int i = 0; i < item_num; i++)
    {
        items[i] = KP_CreateItem(my_can, items_attri[i].m_id, items_attri[i].s_id, items_attri[i].type, 0, items_attri[i].buff_size);
    }   

    s_BOARD u_board={
        .busname = "can0",
        .baudrate = "1MB",
    };
    system("ifconfig can0 down");
    system("ip link set can0 up type can bitrate 1000000 triple-sampling on");
    can0 = canOpen(&u_board,can0ReceiveThread);
    if(can0 == NULL){
        perror("canOpen err");
        goto error;
    }



    rd = r_bootm_session_init(u_r_bootm_output_stream,u_r_bootm_log,EnterMutex,LeaveMutex);
    if(rd <=0 ){
        goto error;
    }
    printf("r_bootm_session_init ok\n");



    pthread_t tid;
    ret = pthread_create(&tid, NULL, u_r_boot_m_timer_Thread, NULL);
    if(ret){
        perror("u_r_boot_m_timer_Thread create err");
        goto error;
    }
    char *r_boot_ret_buffer;
    int r_boot_ret_size;
    r_boot_ret_buffer = r_bootm_list_all(rd,&r_boot_ret_size,3);
    if(r_boot_ret_buffer == NULL) {
        printf("r_bootm_list_all err\n");
    }else{
        //printf("r_bootm_list_all ok\n%s",r_boot_ret_buffer);
        free(r_boot_ret_buffer);
    }

    char *test_file_path = "./test.bin";
	int fd = open(test_file_path, O_RDONLY);
	int file_size = lseek(fd,0,SEEK_END);
	ftp_buffer = (char *)malloc(file_size);
	lseek(fd,0,SEEK_SET);
	read(fd,ftp_buffer,file_size);
	close(fd);

    if(r_bootm_ftp_sync(rd, "new_app.bin",ftp_buffer,file_size,3) == false){
        printf("r_bootm_ftp_sync error\n%s");
    }else{
        printf("r_bootm_ftp_sync ok\n%s");
    }
    

    
    while(1){
        // if(r_bootm_ftp_async(rd, 
        // "new_app.bin", 
        // ftp_buffer, file_size, 
        // u_ftp_ok_callback, u_ftp_error_callback) == false){
        //     //printf("r_bootm_ftp_async start failed\n");
        // }else{
        //     printf("r_bootm_ftp_async start ok\n");
        // }
    }
    
    
error:
    KP_DeInit(my_can);
    canClose(can0);
    return 1;

}
