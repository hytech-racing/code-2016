#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>

#include "can_lib.h"
#include "bt_lib.h"

#define RED_CONSOLE "\033[31;1m"
#define GREEN_CONSOLE "\033[32;1m"
#define YELLOW_CONSOLE "\033[33;1m"
#define BLUE_CONSOLE "\033[34;1m"
#define NO_COLOR_CONSOLE "\033[0m"

#define TIME_LEFT_HIST_LEN 100

const std::string CONSOLE_COLORS[] = {RED_CONSOLE, GREEN_CONSOLE,
                                      YELLOW_CONSOLE, BLUE_CONSOLE};

// variables
struct timeval prev_time;
struct timeval curr_time;
uint16_t amp_history[TIME_LEFT_HIST_LEN];
uint8_t amp_insert_pos = 0;
int running_amp_sum = 0;

// function prototypes
int process_data_for_sending(uint8_t* bt_data, canframe_t* frame);
void print(canframe_t* frame);

int main() {
    CAN can;
    BT bt(10);
    bt.connect();

    bzero(amp_history, TIME_LEFT_HIST_LEN * sizeof(uint16_t));
    gettimeofday(&prev_time, NULL);
    curr_time = prev_time;

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
    uint16_t value;
    bzero(bt_data, BT::DATA_LENGTH);
    switch(frame->can_id) {
        case 0x01:
            // State of Charge
            value = frame->data[1] / 2;
            bt_data[0] = 0;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0x02:
            // Time Left (0x02, 6-7)
            value = ((frame->data[7] << 8) | frame->data[6]);
            value /= (running_amp_sum / 100);
            bt_data[0] = 1;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0x04:
            // Avg and High Battery Temp (0x04, 0,2)
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
            // Motor Temp (0xA2, 4-5)
            value = ((frame->data[5] << 8) | frame->data[4]) / 10;
            bt_data[0] = 4;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0xA5:
            // TODO CALCULATIONS
            // Speed (0xA5, 2-3)
            // RPM * (16/35) * 5.2 = feet / min
            // * 60/5280 = mph
            // Multiply by 10 to send
            // Circumference = 5.2 ft
            value = ((frame->data[3] << 8) | frame->data[2]);
            value *= (uint16_t) ((16.0/35.0) * 5.2 * (60.0/5280.0) * 10);
            bt_data[0] = 5;
            memcpy(&bt_data[1], &value, sizeof(value));
            break;
        case 0xA6:
            // Current current draw (0xA6, 6-7)
            value = ((frame->data[7] << 8) | frame->data[6]);
            gettimeofday(&curr_time, NULL);
            if ((curr_time.tv_sec - prev_time.tv_sec) > 0) {
                running_amp_sum -= amp_history[amp_insert_pos];
                running_amp_sum += value;
                amp_history[amp_insert_pos++] = value;
                amp_insert_pos %= TIME_LEFT_HIST_LEN;
                prev_time = curr_time;
            }
            return -1;
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
