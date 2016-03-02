#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"


static __u32 dropcnt;
static __u32 last_dropcnt;

struct can_frame* get_frame(int s, struct sockaddr_can *addr) {
    struct timeval *last_tv = (struct timeval*) malloc(sizeof(struct timeval));
    struct timeval *tv = (struct timeval*) malloc(sizeof(struct timeval));
    last_tv->tv_sec = 0;
    last_tv->tv_usec = 200000;
    struct can_frame *frame = (struct can_frame*) malloc(sizeof(struct can_frame));
    struct iovec *iov = (struct iovec*) malloc(sizeof(struct iovec));
    struct msghdr *msg = (struct msghdr*) malloc(sizeof(struct msghdr));
    struct cmsghdr *cmsg = (struct cmsghdr*) malloc(sizeof(struct cmsghdr));
    fd_set rdfs;
    int nbytes;
    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];

    /* these settings are static and can be held out of the hot path */
    iov->iov_base = frame;
    msg->msg_name = addr;
    msg->msg_iov = iov;
    msg->msg_iovlen = 1;
    msg->msg_control = ctrlmsg;

    FD_ZERO(&rdfs);
    FD_SET(s, &rdfs);
    int ret = select(s + 1, &rdfs, NULL, NULL, last_tv);
    if (ret < 0) {
        printf("select failed\n");
    }

    if (FD_ISSET(s, &rdfs)) {
        int idx;

        /* these settings may be modified by recvmsg() */
        iov->iov_len = sizeof(*frame);
        msg->msg_namelen = sizeof(*addr);
        msg->msg_controllen = sizeof(*ctrlmsg);
        msg->msg_flags = 0;

        nbytes = recvmsg(s, msg, 0);
        /*
         * printf("nbytes: %d, sizeof struct: %d\n", nbytes, sizeof(struct can_frame));
        */
        fflush(stdout);
        if (nbytes < 0) {
            perror("read");
            return NULL;
        }

        if (nbytes < sizeof(struct can_frame)) {
            printf("read: incomplete CAN frame %d\n", nbytes);
            fflush(stdout);
            return NULL;
        }

        for (cmsg = CMSG_FIRSTHDR(msg); cmsg && (cmsg->cmsg_level == SOL_SOCKET); cmsg = CMSG_NXTHDR(msg,cmsg)) {
            if (cmsg->cmsg_type == SO_TIMESTAMP)
                tv = (struct timeval *)CMSG_DATA(cmsg);
            else if (cmsg->cmsg_type == SO_RXQ_OVFL)
                dropcnt = *(__u32 *)CMSG_DATA(cmsg);
        }


        /* check for (unlikely) dropped frames on this specific socket */
        if (dropcnt != last_dropcnt) {

            __u32 frames;

            if (dropcnt > last_dropcnt)
                frames = dropcnt - last_dropcnt;
            else
                frames = 4294967295U - last_dropcnt + dropcnt; /* 4294967295U == UINT32_MAX */

            last_dropcnt = dropcnt;
        }
    }
    return frame;
}

