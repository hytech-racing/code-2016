#include <chrono>
#include <ctime>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
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
void log_to_file(canframe_t* frame);

std::ofstream log_mc;
std::ofstream log_bms;
std::ofstream log_main_ard;
std::ofstream log_evdc;
std::ofstream log_rear_ard;
struct timeval log_mc_prev_time;
struct timeval log_bms_prev_time;
struct timeval log_main_ard_prev_time;
struct timeval log_evdc_prev_time;
struct timeval log_rear_ard_prev_time;
char timestr[20];

int main() {
    log_mc.open("logs_mc.txt", std::ios::out | std::ios::app);
    log_bms.open("logs_bms.txt", std::ios::out | std::ios::app);
    log_main_ard.open("logs_main_arduino.txt", std::ios::out | std::ios::app);
    log_evdc.open("logs_evdc.txt", std::ios::out | std::ios::app);
    log_rear_ard.open("logs_read_arduino.txt", std::ios::out | std::ios::app);

    gettimeofday(&log_mc_prev_time, NULL);
    gettimeofday(&log_bms_prev_time, NULL);
    gettimeofday(&log_main_ard_prev_time, NULL);
    gettimeofday(&log_evdc_prev_time, NULL);
    gettimeofday(&log_rear_ard_prev_time, NULL);

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
        } else {
            if (0 == process_data_for_sending(bt_buffer, frame)) {
                bt.send(bt_buffer);
            }
            log_to_file(frame);
        }
    }

    free(frame);
    bt.disconnect();
    log_mc.close();
    log_bms.close();
    log_main_ard.close();
    log_evdc.close();
    log_rear_ard.close();
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
            //value *= (uint16_t) ((16.0/35.0) * 5.2 * (60.0/5280.0) * 10);
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

void log_to_file(canframe_t* frame) {
    std::ofstream* out_file;
    gettimeofday(&curr_time, NULL);
    if (frame->can_id < 0x10
            && (curr_time.tv_sec - log_bms_prev_time.tv_sec) > 0) {
        out_file = &log_bms;
        log_bms_prev_time = curr_time;
    } else if (frame->can_id < 0x30
            && (curr_time.tv_sec - log_rear_ard_prev_time.tv_sec) > 0) {
        out_file = &log_rear_ard;
        log_rear_ard_prev_time = curr_time;
    } else if (frame->can_id < 0x90
            && (curr_time.tv_sec - log_main_ard_prev_time.tv_sec) > 0) {
        out_file = &log_main_ard;
        log_main_ard_prev_time = curr_time;
    } else if (frame->can_id < 0xB0
            && (curr_time.tv_sec - log_mc_prev_time.tv_sec) > 0) {
        out_file = &log_mc;
        log_mc_prev_time = curr_time;
    } else if (frame->can_id < 0xD0
            && (curr_time.tv_sec - log_evdc_prev_time.tv_sec) > 0) {
        out_file = &log_evdc;
        log_evdc_prev_time = curr_time;
    } else {
        return;
    }

    std::time_t timestamp = std::chrono::system_clock
        ::to_time_t(std::chrono::system_clock::now());
    if (std::strftime(timestr, sizeof(timestr), "%T",
                std::localtime(&timestamp))) {
        *out_file << timestr << ": ";
    }
    *out_file << frame->can_id << ": ";
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t data = frame->data[i];
        *out_file << (int) data << " ";
    }
    *out_file << std::endl;
}
