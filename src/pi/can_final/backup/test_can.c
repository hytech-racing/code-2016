#include "candump_lib.c"
#include "time.h"

static volatile bool running = true;

void sigterm(int signo) {
    running = false;
}

int main() {
    struct sockaddr_can addr;
    struct ifreq ifr;

    signal(SIGTERM, sigterm);
    signal( SIGHUP, sigterm);
    signal( SIGINT, sigterm);

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        printf("socket error\n");
        return 1;
    }
    addr.can_family = AF_CAN;

    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    strcpy(ifr.ifr_name, "can0");

    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;

    int ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    struct can_frame *frame;
    while (running) {
        frame = get_frame(s, &addr);
        if (frame) {
            printf("ID: %d\n", frame->can_id);
            fflush(stdout);
        }
        usleep(100000);
    }

    return 0;
}
