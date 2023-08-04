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


#include "uart.h"


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "r_boot_master.h"
#include "stream_packer.h"


PUart uart3 = NULL;
int rd=0;
char *ftp_buffer = NULL;



upacker_inst u_packer;

extern int rd;

void PACKER_CB_s(uint8_t *d, uint16_t s)
{

    uart_send(uart3,d,s);

}

void PACKER_CB_h(uint8_t *d, uint16_t s)
{
    r_bootm_despatch(rd, d,s);
}

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
 * @param buffer 
 * @param buffer_size 
 * @return true 
 * @return false 
 */
bool u_r_bootm_output_stream(char * buffer, int buffer_size)
{
    upacker_pack(&u_packer, buffer, buffer_size);
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

} 
/**
 * @brief 
 * 
 */
static void u_ftp_error_callback(void)
{
    printf("transmit test.bin error\n");

}

/**
 * @brief Receive any length of serial port data through the signal handler, and integrate to a whole frame
 * 
 * @param sig 
 * @param info 
 * @param context 
 */
void Uart3_Handler(int sig, siginfo_t *info, void *context)
{   
    uint8_t buffer[512];
    int buffer_size = 0;

    if(SIGRTMIN != sig){
        return;
    }
    
    if(POLL_IN == info->si_code){
        buffer_size = read(uart3->ttyOx_fd, buffer, 512);
        upacker_unpack(&u_packer, buffer, buffer_size);
        
    }
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
    
    pthread_t tid;

    uart3 = uart_open(UART3,115200, Uart3_Handler);
    if(uart3 == NULL){
        perror("uart3 Error:");
        return -1;
    }
    printf("uart3 open ok\n");
    upacker_init(&u_packer, PACKER_CB_h, PACKER_CB_s);
    printf("upacker_init ok\n");
    rd = r_bootm_session_init(u_r_bootm_output_stream,u_r_bootm_log, NULL, NULL);
    if(rd <=0 ){
        goto error;
    }
    printf("r_bootm_session_init ok\n");
    ret = pthread_create(&tid, NULL, u_r_boot_m_timer_Thread, NULL);
    if(ret){
        perror("u_r_boot_m_timer_Thread create err");
        goto error;
    }
    // char *r_boot_ret_buffer;
    // int r_boot_ret_size;
    // r_boot_ret_buffer = r_bootm_list_all(rd,&r_boot_ret_size,3);
    // if(r_boot_ret_buffer == NULL) {
    //     printf("r_bootm_list_all err\n");
    // }else{
    //     printf("r_bootm_list_all ok\n%s",r_boot_ret_buffer);
    //     free(r_boot_ret_buffer);
    // }

    char *test_file_path = "./test.bin";
	int fd = open(test_file_path, O_RDONLY);
	int file_size = lseek(fd,0,SEEK_END);
	ftp_buffer = (char *)malloc(file_size);
	lseek(fd,0,SEEK_SET);
	read(fd,ftp_buffer,file_size);
	close(fd);

    // if(r_bootm_ftp_sync(rd, "new_app.bin",ftp_buffer,file_size,3) == false){
    //     printf("r_bootm_ftp_sync error\n%s");
    // }else{
    //     printf("r_bootm_ftp_sync ok\n%s");
    // }
    

    
    while(1){
        if(r_bootm_ftp_async(rd, 
        "new_app.bin", 
        ftp_buffer, file_size, 
        u_ftp_ok_callback, u_ftp_error_callback) == false){
            //printf("r_bootm_ftp_async start failed\n");
        }else{
            printf("r_bootm_ftp_async start ok\n");
        }
    }
    
    
error:
    uart_close(uart3);
    return 1;

}
