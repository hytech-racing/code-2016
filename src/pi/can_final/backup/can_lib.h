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

typedef struct sockaddr_can sockaddrcan_t;
typedef struct can_frame canframe_t;
typedef struct msghdr msghdr_t;


class MCP_CAN {
public:
    MCP_CAN();
    canframe_t* read();
private:
    int sock;
    sockaddrcan_t addr;
    canframe_t frame;
    msghdr_t msg;
    fd_set rdfs;
    struct iovec iov;
    struct ifreq ifr;
};
