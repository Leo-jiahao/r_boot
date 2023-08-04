/**
 * @file dev_can.c
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

#include <stdlib.h>
#include <string.h>


/**
 * @brief 
 * 
 * @param board 
 * @param port 
 * @param canRcvThread 
 * @return CAN_PORT 
 */
CAN_PORT canOpen(s_BOARD *board, canReceiveThread canRcvThread)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    int err;
    int ret;
    CAN_PORT fd0 = (CAN_PORT)malloc (sizeof (CANPort));


    if(!fd0)
    {
        return NULL;
    }

    fd0->fd = CAN_SOCKET (PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd0->fd < 0)
    {
        fprintf (stderr, "Socket creation failed: %s\n",
            strerror (CAN_ERRNO (fd0->fd)));
        goto error_ret;
    }

    if (*board->busname >= '0' && *board->busname <= '9')
        snprintf (ifr.ifr_name, IFNAMSIZ, CAN_IFNAME, board->busname);
    else
        strncpy (ifr.ifr_name, board->busname, IFNAMSIZ);
    err = CAN_IOCTL (fd0->fd, SIOCGIFINDEX, &ifr);
    if (err)
        {
        fprintf (stderr, "Getting IF index for %s failed: %s\n",
            ifr.ifr_name, strerror (CAN_ERRNO (err)));
        goto error_close;
        }
    
    {
        int loopback = 1;
        err = CAN_SETSOCKOPT(fd0->fd, SOL_CAN_RAW, CAN_RAW_LOOPBACK,
                &loopback, sizeof(loopback));
        if (err) {
            fprintf(stderr, "rt_dev_setsockopt: %s\n", strerror (CAN_ERRNO (err)));
            goto error_close;
        }
    }
  
  
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    err = CAN_BIND (fd0->fd, (struct sockaddr *) &addr, sizeof (addr));
    if (err)
    {
        fprintf (stderr, "Binding failed: %s\n", strerror (CAN_ERRNO (err)));
        goto error_close;
    }

    fd0->used = 1;

    ret = pthread_create(&fd0->tid, NULL, canRcvThread, fd0);

    if(ret){
        fprintf (stderr, "pthread_create:%s\n", strerror (CAN_ERRNO (err)));
        goto error_close;
    }
    return fd0;

error_close:
    CAN_CLOSE (fd0->fd);

error_ret:
    free (fd0);
    return NULL;
}


static void WaitReceiveTaskEnd(pthread_t *Thread)
{
	if(pthread_cancel(*Thread)) {
		perror("pthread_cancel()");
	}
	if(pthread_join(*Thread, NULL)) {
		perror("pthread_join()");
	}
}

/**
 * @brief 
 * 
 * @param port 
 * @return int 
 */
int canClose(CAN_PORT port)
{

    if (port)
    {
        port->used = 0;
        CAN_CLOSE (port->fd);

        WaitReceiveTaskEnd(&port->tid);

        free (port);

    }
  return 0;
}
/**
 * @brief 
 * 
 * @param port 
 * @param m 
 * @return uint8_t 
 */
uint8_t canSend(CAN_PORT port, Message *m)
{
    if(!port){
        return 1;
    }
    int res;
    struct can_frame frame;

    frame.can_id = m->cob_id;
    if (frame.can_id >= 0x800)
        frame.can_id |= CAN_EFF_FLAG;
    frame.can_dlc = m->len;
    if (m->rtr)
        frame.can_id |= CAN_RTR_FLAG;
    else
        memcpy (frame.data, m->data, 8);

    #if defined DEBUG_MSG_CONSOLE_ON
    MSG("out : ");
    print_message(m);
    #endif
    printf("canSend frame_id:0X%4X, dlc:%d ", frame.can_id, frame.can_dlc);
    for (int i = 0; i < frame.can_dlc; i++)
    {
        printf("0X%2X ",frame.data[i]);
            /* code */
    }
    printf("\n");
    res = CAN_SEND (((CANPort*)port)->fd, &frame, sizeof (frame), 0);
    
    if (res < 0){
        fprintf (stderr, "Send failed: %s\n", strerror (CAN_ERRNO (res)));
        return 1;
    }

    return 0;
}

