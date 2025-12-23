// src/protocol_i2c.c
// I2C protocol implementation (placeholder)
#include "bootloader.h"
#include <stdio.h>

void i2c_init(void) {
    printf("[I2C] Initialized at 100 kHz, address 0x50\n");
}

void i2c_start(void) {
    printf("[I2C] Start condition\n");
}

void i2c_stop(void) {
    printf("[I2C] Stop condition\n");
}

uint8_t i2c_write(uint8_t data) {
    printf("[I2C] Write: 0x%02X\n", data);
    return 0; // ACK
}