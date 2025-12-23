// src/protocol_spi.c
// SPI protocol implementation (placeholder)
#include "bootloader.h"
#include <stdio.h>

void spi_init(void) {
    printf("[SPI] Initialized (Mode 0, 1 MHz)\n");
}

void spi_select(void) {
    printf("[SPI] CS# asserted\n");
}

void spi_deselect(void) {
    printf("[SPI] CS# de-asserted\n");
}

uint8_t spi_transfer(uint8_t data) {
    printf("[SPI] Transfer: 0x%02X\n", data);
    return 0;
}