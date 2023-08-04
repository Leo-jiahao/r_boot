/**
 * @file dev_can.h
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


#ifndef DEV_CAN_H_
#define DEV_CAN_H_

#include <stdint.h>
#include <pthread.h>
#include "linux/can.h"
#include "linux/can/raw.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#ifndef __USE_MISC
#define __USE_MISC
#endif

#include "net/if.h"

#define CAN_IFNAME     "can%s"
#define CAN_SOCKET     socket
#define CAN_CLOSE      close
#define CAN_RECV       recv
#define CAN_SEND       send
#define CAN_BIND       bind
#define CAN_IOCTL      ioctl
#define CAN_ERRNO(err) errno
#define CAN_SETSOCKOPT setsockopt


struct struct_s_BOARD {
  char * busname;  /**< The bus name on which the CAN board is connected */
  char * baudrate; /**< The board baudrate */
};
typedef struct struct_s_BOARD s_BOARD;

typedef struct
{
  pthread_t tid;
  int fd;
  void* d;
  char used;
} CANPort, *CAN_PORT; /* taken from drivers/unix.c */

typedef void *(*canReceiveThread)(void * port);

struct Message {
  uint16_t cob_id;	/**< message's ID */
  uint8_t rtr;		/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
  uint8_t len;		/**< message's length (0 to 8) */
  uint8_t data[8]; /**< message's datas */
};

typedef struct Message Message;


CAN_PORT canOpen(s_BOARD *board, canReceiveThread canRcvThread);

int canClose(CAN_PORT port);

uint8_t canSend(CAN_PORT port, Message *m);
#endif /* CAN_H_ */

