#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>

#include "can_lib.h"

MCP_CAN::MCP_CAN() {
    // configure CAN address and other parameters
    addr.can_family = AF_CAN;
    bzero(&ifr.ifr_name, sizeof(ifr.ifr_name));
    strcpy(ifr.ifr_name, "can0");
    addr.can_ifindex = ifr.ifr_ifindex;

    // setup framework for reading CAN messages
    iov.iov_base    = &frame;
    msg.msg_name    = &addr;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;
}

MCP_CAN::initialize() {
    // initialize socket
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    // bind CAN socket
    bind(sock, (struct sockaddr *) &addr, sizeof(addr));
}

canframe_t* MCP_CAN::read() {
    int nbytes;

    FD_ZERO(&rdfs);
    FD_SET(sock, &rdfs);
    if (select(sock + 1, &rdfs, nullptr, nullptr, &timeout) < 0) {
        printf("select failed\n");
    }

    if (FD_ISSET(sock, &rdfs)) {
        iov.iov_len = sizeof(frame);
        msg.msg_namelen = sizeof(addr);
        msg.msg_flags = 0;

        nbytes = recvmsg(sock, &msg, 0);
        if (nbytes < 0) {
            perror("read");
            return nullptr;
        }

        if (nbytes < sizeof(canframe_t)) {
            printf("read: incomplete CAN frame %d\n", nbytes);
            fflush(stdout);
            return nullptr;
        }
    }
    return &frame;
}

int MCP_CAN::send(int id, unsigned char *data, uint8_t msg_len) {
    bzero(&frame, sizeof(canframe_t));
    frame.can_id = id;
    memcpy(frame.data, data, 8);
    frame.can_dlc = msg_len;
    int nbytes = write(sock, &frame, sizeof(canframe_t));
    if (nbytes != sizeof(frame)) {
        printf("send failed: nbytes = %d, sizeof = %d", nbytes,
                sizeof(frame));
        return 1;
    }
    return 0;
}
