#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "can_lib.h"
#include "bt_lib.h"

#define RED_CONSOLE "\033[31;1m"
#define GREEN_CONSOLE "\033[32;1m"
#define YELLOW_CONSOLE "\033[33;1m"
#define BLUE_CONSOLE "\033[34;1m"
#define NO_COLOR_CONSOLE "\033[0m"

const std::string CONSOLE_COLORS[] = {RED_CONSOLE, GREEN_CONSOLE,
                                      YELLOW_CONSOLE, BLUE_CONSOLE};

// function prototypes
int process_data_for_sending(uint8_t* bt_data, canframe_t* frame);
void print(canframe_t* frame);

int main() {
    CAN can;
    BT bt(10);
    bt.connect();

    canframe_t *frame = (canframe_t*) malloc(sizeof(canframe_t));
    uint8_t bt_buffer[BT::DATA_LENGTH];

    while (1) {
        if (can.read(frame) > 0) {
            std::cout << "Error reading message" << std::endl;
        }
        if (!process_data_for_sending(bt_buffer, frame)) {
            bt.send(bt_buffer);
        }
        usleep(10000);
    }

    free(frame);
    bt.disconnect();
    return 0;
}

int process_data_for_sending(uint8_t* bt_data, canframe_t* frame) {
    short value;
    bzero(bt_data, BT::DATA_LENGTH);
    switch(frame->can_id) {
        case 0x01:
            value = frame->data[1] / 2;
            bt_data[0] = 0;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0x02:
            // TODO CALCULATIONS
            // Time Left (0x02, 6-7)
            break;
        case 0x04:
            bt_data[0] = 2;
            memcpy(&bt_data[1], &frame->data[2], sizeof(uint8_t));
            memcpy(&bt_data[2], &frame->data[0], sizeof(uint8_t));
            break;
        case 0x10:
            // TODO TALK TO ANDREW
            // Startup State (0x10, 0)
            // Error Messages (0x10, 1)
            break;
        case 0xA2:
            value = ((frame->data[5] << 8) | frame->data[4]) / 10;
            bt_data[0] = 4;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0xA5:
            // TODO CALCULATIONS
            // Speed (0xA5, 2-3)
            break;
        default:
            return -1;
    }
    return 0;
}

void print(canframe_t* frame) {
    if (frame && frame->can_dlc > 0) {
        for (uint8_t i = 0; i < frame->can_dlc; ++i) {
            std::cout << CONSOLE_COLORS[rand() % 4];
            std::cout << (int) frame->data[i] << " ";
        }
        std::cout << NO_COLOR_CONSOLE << std::endl;
    } else {
        std::cout << "Can frame is NULL or has an empty message" << std::endl;
    }
}
