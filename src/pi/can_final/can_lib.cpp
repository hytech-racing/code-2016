#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>

#include "can_lib.h"

MCP_CAN::MCP_CAN() {
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    addr.can_family = AF_CAN;
    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    strcpy(ifr.ifr_name, "can0");
    ioctl(sock, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *) &addr, sizeof(addr));

    iov.iov_base    = &frame;
    msg.msg_name    = &addr;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;
}

canframe_t* MCP_CAN::read() {
    int nbytes;

    FD_ZERO(&rdfs);
    FD_SET(sock, &rdfs);
    int ret = select(sock + 1, &rdfs, nullptr, nullptr, 0);
    if (ret < 0) {
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

int MCP_CAN::send(int id, unsigned char *data) {
    canframe_t *toSend;

    memset(toSend, 0, sizeof(toSend));
    toSend->can_id = id;
    memcpy(toSend->data, data, 8);
    int nbytes = write(sock, toSend, sizeof(*toSend));
    if (nbytes != sizeof(*toSend)) {
        printf("send failed: nbytes = %d, sizeof = %d", nbytes,
                sizeof(*toSend));
        fflush(stdout);
        return 1;
    }
    return 0;
}
