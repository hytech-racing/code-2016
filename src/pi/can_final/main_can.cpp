#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "can_lib.h"

int main() {
    MCP_CAN can;

    canframe_t *frame;
    uint8_t buffer[] = {70, 85, 67, 75, 32, 89, 79, 85};
    while (1) {
        frame = can.read();
        if (frame && frame->can_dlc > 0) {
            for (uint8_t i = 0; i < frame->can_dlc; ++i) {
                printf("%d ", frame->data[i]);
            }
            printf("\n");
        }

        can.send(0x60, buffer, 8);
        usleep(100000);
    }

    return 0;
}
