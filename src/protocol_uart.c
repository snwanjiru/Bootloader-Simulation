// src/protocol_uart.c
// UART protocol implementation (placeholder)
#include "bootloader.h"
#include <stdio.h>

// Simple UART simulation functions
void uart_init(void) {
    printf("[UART] Initialized at 115200 baud\n");
}

void uart_send_byte(uint8_t data) {
    printf("[UART TX] 0x%02X\n", data);
}

uint8_t uart_receive_byte(void) {
    // In simulation, we use console input instead
    return 0;
}