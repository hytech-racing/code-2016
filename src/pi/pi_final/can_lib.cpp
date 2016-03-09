#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>

#include "can_lib.h"

MCP_CAN::MCP_CAN() {
    // initialize CAN socket and configure CAN address and other parameters
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    addr.can_family = AF_CAN;
    bzero(&ifr.ifr_name, sizeof(ifr.ifr_name));
    strcpy(ifr.ifr_name, "can0");
    ioctl(sock, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;

    // bind CAN socket
    bind(sock, (struct sockaddr *) &addr, sizeof(addr));

    // setup framework for reading CAN messages
    iov.iov_base    = &recv_frame;
    msg.msg_name    = &addr;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;
}

int MCP_CAN::read(canframe_t* read_frame) {
    int nbytes;

    FD_ZERO(&rdfs);
    FD_SET(sock, &rdfs);
    if (select(sock + 1, &rdfs, nullptr, nullptr, &timeout) < 0) {
        printf("select failed\n");
        return 1;
    }

    if (FD_ISSET(sock, &rdfs)) {
        iov.iov_len = sizeof(recv_frame);
        msg.msg_namelen = sizeof(addr);
        msg.msg_flags = 0;

        nbytes = recvmsg(sock, &msg, 0);
        if (nbytes < 0) {
            return 2;
        }

        if (nbytes < sizeof(canframe_t)) {
            return 3;
        }
    }

    memcpy(read_frame, &recv_frame, sizeof(canframe_t));
    return 0;
}

int MCP_CAN::send(int id, unsigned char *data, uint8_t msg_len) {
    bzero(&send_frame, sizeof(canframe_t));
    send_frame.can_id = id;
    memcpy(send_frame.data, data, 8);
    send_frame.can_dlc = msg_len;

    if (write(sock, &send_frame, sizeof(canframe_t)) != sizeof(canframe_t)) {
        return 1;
    }
    return 0;
}