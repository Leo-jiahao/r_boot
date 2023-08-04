/**
 * @file uart.h
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


#ifndef UART_H_
#define UART_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <signal.h>

#define UART_PATH_LEN_MAX (64)

typedef enum UART_X_ /*定义UARTx*/
{
    UART0,                             /*not available*/
    UART1,                             /*TXD -> P9_24, RXD->P9_26*/
    UART2,                             /*TXD -> P9_21, RXD->P9_22*/
    UART3,                             /*not available*/
    UART4,                             /*TXD -> P9_13, RXD->P9_11*/
    UART5 /*TXD -> P8_37, RXD->P8_38*/ /*测试使未通过,提示为File exists,但是没有找到tttyO5文件,暂时不清楚为什么*/
} Uart_x;

typedef struct UART_
{
    Uart_x uart_x;  /*串口号*/
    long baud_rate; /*波特率*/
    int ttyOx_fd; /*串口操作fd*/
} Uart, *PUart;



PUart uart_open(Uart_x uart_x, unsigned int baude_rate, void (*Uart_Handler)(int , siginfo_t *, void *));
void uart_close(PUart p_uart);

int uart_send(PUart p_uart, const char *send_buf, int len);
int uart_receive(PUart p_uart, char *recv_buf, int len);

/*串口格式化输出函数*/
void uart_printf(PUart p_uart, const char *format, ...);



extern PUart g_p_uart;
#endif /* UART_H_ */