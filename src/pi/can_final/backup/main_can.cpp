#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "can_lib.h"

int main() {
    MCP_CAN can;

    canframe_t *frame;
    while (1) {
        frame = can.read();
        if (frame) {
            for (uint8_t i = 0; i < frame->can_dlc; ++i) {
                printf("%d ", frame->data[i]);
            }
            printf("\n");
        }
        usleep(100000);
    }

    return 0;
}
