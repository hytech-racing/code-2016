#include "can_lib.h"
#include "time.h"

static volatile bool running = true;

void sigterm(int signo) {
    running = false;
}

int main() {
    signal(SIGTERM, sigterm);
    signal( SIGHUP, sigterm);
    signal( SIGINT, sigterm);

    MCP_CAN can;

    canframe_t *frame;
    while (running) {
        frame = can.read();
        if (frame) {
            printf("ID: %d\n", frame->can_id);
            fflush(stdout);
        }
        usleep(100000);
    }

    return 0;
}
