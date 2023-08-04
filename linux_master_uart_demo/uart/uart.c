/**
 * @file uart.c
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

#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAXBUF (15)
#define DEFAULT_PRECI (3) //符点型默认精度


/**
 * @brief set uart to async read
 * 
 * @param p_uart 
 */
static void async_io_init(PUart p_uart, void (*Uart_Handler)(int sig, siginfo_t *info, void *context))
{
    struct sigaction sigatn;
    int flag;
    /* 使能异步 I/O */
    flag = fcntl(p_uart->ttyOx_fd, F_GETFL); //使能串口的异步 I/O 功能
    flag |= O_ASYNC;
    fcntl(p_uart->ttyOx_fd, F_SETFL, flag);

    /* 设置异步 I/O 的所有者 */
    fcntl(p_uart->ttyOx_fd, F_SETOWN, getpid());

    /* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
    fcntl(p_uart->ttyOx_fd, F_SETSIG, SIGRTMIN);
    
    /* 为实时信号 SIGRTMIN 注册信号处理函数 */
    sigatn.sa_sigaction = Uart_Handler; //当串口有数据可读时，会跳转到 io_handler 函数
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN, &sigatn, NULL);
}
/**
 * @brief get baude rate
 * 
 * @param baude_rate 
 * @return unsigned int 
 */
static unsigned int GET_BaudeRate(unsigned int baude_rate)
{
    speed_t speed;
    switch (baude_rate) {
        case 1200: speed = B1200;
        break;
        case 1800: speed = B1800;
        break;
        case 2400: speed = B2400;
        break;
        case 4800: speed = B4800;
        break;
        case 9600: speed = B9600;
        break;
        case 19200: speed = B19200;
        break;
        case 38400: speed = B38400;
        break;
        case 57600: speed = B57600;
        break;
        case 115200: speed = B115200;
        break;
        case 230400: speed = B230400;
        break;
        case 460800: speed = B460800;
        break;
        case 500000: speed = B500000;
        break;
        default: //默认配置为 115200
        speed = B115200;
        printf("default baud rate: 115200\n");
        break;
    }
    return speed; 
}

static struct termios old_cfg;
/**
 * @brief uart_open default dbit = 8, parity = N, sbit = 1,async mode
 *        
 * @param uart_x UART1/UART2/UART4/UART5, usual UART4
 * @param baude_rate 9600,19200,115200...
 * @param callback receive callback 
 * @return PUart 
 * ERR NULL
 * others ok
 */
PUart uart_open(Uart_x uart_x, unsigned int baude_rate, void (*Uart_Handler)(int sig, siginfo_t *info, void *context))
{
    /*定义指向termios 结构类型的指针opt*/
    struct termios new_opt={0};  

    PUart p_uart = (PUart)malloc(sizeof(Uart));

    if (p_uart == NULL)
    {
        fprintf(stderr, "Error:Create GPIO failed, failed to allocate memory!\n");
        return NULL;
    }

    p_uart->uart_x = uart_x;
    p_uart->baud_rate = GET_BaudeRate(baude_rate);
    
    
    /*创建串口读写fd*/
    // char ttyOx_path[UART_PATH_LEN_MAX] = "/dev/ttyO"; 
    char ttyOx_path[UART_PATH_LEN_MAX] = "/dev/ttymxc"; 
    char uart_no[10] = {0};
    snprintf(uart_no, sizeof(uart_no), "%d", uart_x-1);
    strcat(ttyOx_path, uart_no);

    int fd = open(ttyOx_path, O_RDWR | O_NOCTTY);
    if (-1 == fd)
    {
        fprintf(stderr, "Error:can not open /dev/ttyO%d.%s\n", uart_x, strerror(errno));

        free(p_uart);
        
        return NULL;
    }
    
    /*get old cfg*/
    if (0 > tcgetattr(fd, &old_cfg)) {
        fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
        close(fd);
        return NULL;
    }

    p_uart->ttyOx_fd = fd;

    /*设定波特率*/
    

    cfmakeraw(&new_opt);//raw mode

    new_opt.c_cflag |= CREAD;//enable read
    

    if (-1 == cfsetspeed(&new_opt, p_uart->baud_rate)) /*指定输入波特率*/
    {
        fprintf(stderr, "Buaderate error: wrong buaderate! %s\n", strerror(errno));
        
        free(p_uart);
        
        return NULL;
    }
    
    new_opt.c_cflag &= ~CSIZE;
    new_opt.c_cflag |= CS8;//8bits 
    new_opt.c_cflag &= ~PARENB;//no check
    new_opt.c_iflag &= ~INPCK;
    new_opt.c_cflag &= ~CSTOPB;//1bit stop
    
    new_opt.c_cc[VTIME] = 0;
    new_opt.c_cc[VMIN] = 0;
    
    //clear uart buffer
    if(tcflush(fd, TCIOFLUSH) <0){
        fprintf(stderr, "tcflush error: %s\n", strerror(errno));
        
        free(p_uart);
        
        return NULL;
    }

    if(tcsetattr(fd, TCSANOW, &new_opt) <0) //将修改后的termios数据设置到串口中
    {
        fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
        
        free(p_uart);
        
        return NULL;
    }
    async_io_init(p_uart, Uart_Handler);

    
    return p_uart;
}

/*****************************************************************************
 *
 * 函数名:	uart_close
 * 功能:	关闭串口
 * 参数:
 * 返回值: 无
 * 作者:
 * 修改时间:
 * 备注:由于直接向slots文件写入来删除串口使用shell脚本文件存在BBB退出问题,此处有同样的问题
 * 		由于原因尚不清楚, 此处注释掉卸载函数
 * ***************************************************************************/
void uart_close(PUart p_uart)
{
    if (p_uart == NULL)
    {
        printf("\nDelete UART failed, the UART's address is NULL");
        return;
    }
    tcsetattr(p_uart->ttyOx_fd, TCSANOW, &old_cfg); 
    close(p_uart->ttyOx_fd);
    
    free(p_uart);
}

/*****************************************************************************
 *
 * 函数名:	uart_send
 * 功能:	串口发送数据
 * 参数:	串口, 待发送数据的缓冲区, 发送数据的长度
 * 返回值: 发送成功返回实际发送的字节数,发送失败返回-1
 * 作者:
 * 修改时间:
 * 备注:
 * ***************************************************************************/
int uart_send(PUart p_uart, const char *send_buf, int len)
{
    if (-1 == p_uart->ttyOx_fd)
    {
        printf("\nWrong UART ttyOx fd");
        return -1;
    }
    // printf("uart3 transmit:[");
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%x ",send_buf[i]);
    // }
    // printf("]\n");
    return (write(p_uart->ttyOx_fd, send_buf, len));
}

/*****************************************************************************
 *
 * 函数名:	uart_receive
 * 功能:	串口接收数据
 * 参数:	串口, 数据接收缓冲区, 接收的数据长度
 * 返回值: 接收成功返回字节数,接收失败时返回-1
 * 作者:
 * 修改时间:
 * 备注:
 * ***************************************************************************/
int uart_receive(PUart p_uart, char *recv_buf, int len)
{
    if (-1 == p_uart->ttyOx_fd)
    {
        printf("\nWrong UART ttyOx fd");
        return -1;
    }

    return (read(p_uart->ttyOx_fd, recv_buf, len));
}

/*****************************************************************************
 *
 * 函数名:	uart_printf
 * 功能:	串口格式化输出函数
 * 参数:
 * 返回值: 无
 * 作者:
 * 修改时间:
 * 备注:
 * ***************************************************************************/
void uart_printf(PUart p_uart, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    //STRING
    char *str;

    //OCT
    int value;
    char buf[MAXBUF];

    //FLOAT
    double fdata;
    int f_length = 0;
    int preci = 0;

    while (*format)
    {
        if ('%' != *format) /*输出普通字符*/
        {
            uart_send(p_uart, format++, 1);
            continue;
        }

        switch (*++format)
        {
        case 'c': /*单个字符*/
        {
            str = NULL;
            str = va_arg(ap, char *);
            uart_send(p_uart, str, 1);
        }
        break;
        case 's': /*字符串*/
        {
            str = NULL;
            str = va_arg(ap, char *);
            for (; *str != '\0'; str++)
            {
                uart_send(p_uart, str, 1);
            }

            ++format;
        }
        break;
        case 'd': /*十进制整数*/
        {
            value = va_arg(ap, int);
            snprintf(buf, sizeof(buf), "%d", value);
            for (str = buf; *str != '\0'; str++)
            {
                uart_send(p_uart, str, 1);
            }

            ++format;
        }
        break;
        case 'f': /*浮点型数据*/
        {
            fdata = va_arg(ap, double);
            snprintf(buf, sizeof(buf), "%f", fdata);
            for (str = buf; *str != '\0'; str++)
            {
                uart_send(p_uart, str, 1);
            }

            ++format;
        }
        break;
        case '.': /*自定义浮点型数据*/
        {
            if (isdigit(*++format)) /*小数点后面有数字来规定精度*/
            {
                if ('f' == (*++format))
                {
                    --format; /*当前format存储的是规定精度的数字*/
                    fdata = va_arg(ap, double);
                    char buf2[MAXBUF] = {0};
                    snprintf(buf2, sizeof(buf2), "%f", fdata);

                    /*获取整数部分的字符串长度*/
                    for (f_length = 0; f_length < 10; f_length++)
                    {
                        if (buf2[f_length] == '.') /*获取小数点位置*/
                        {
                            break;
                        }
                    }

                    preci = f_length + 2 + *format - '0';
                    snprintf(buf, preci, "%f", fdata);
                    for (str = buf; *str != '\0'; str++)
                    {
                        uart_send(p_uart, str, 1);
                    }

                    ++format;
                    ++format;
                }
            }
            else
            {
                --format;
                if ('f' == (*++format))
                {
                    fdata = va_arg(ap, double);
                    snprintf(buf, sizeof(buf), "%f", fdata);
                    for (str = buf; *str != '\0'; str++)
                    {
                        uart_send(p_uart, str, 1);
                    }

                    ++format;
                }
            }
        }
        break;
        default:
            break;
        }
    }

    va_end(ap);
}

