#include <net/if.h>
#include <linux/can/raw.h>

#ifndef __CAN_LIB_H__
#define __CAN_LIB_H__

typedef struct sockaddr_can sockaddrcan_t;
typedef struct can_frame canframe_t;
typedef struct msghdr msghdr_t;

class MCP_CAN {
public:
    MCP_CAN();
    canframe_t* read();
    int send(int id, unsigned char *data);

private:
    int sock;
    sockaddrcan_t addr;
    canframe_t frame;
    msghdr_t msg;
    fd_set rdfs;
    struct iovec iov;
    struct ifreq ifr;
};

#endif
