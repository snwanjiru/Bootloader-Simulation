// src/main.c
#include "bootloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    ProtocolType protocol = PROTOCOL_UART;
    
    printf("=== Embedded Bootloader Simulator (Professional) ===\n\n");
    
    // Select protocol
    if (argc > 1) {
        if (strcmp(argv[1], "uart") == 0) {
            protocol = PROTOCOL_UART;
        } else if (strcmp(argv[1], "spi") == 0) {
            protocol = PROTOCOL_SPI;
        } else if (strcmp(argv[1], "i2c") == 0) {
            protocol = PROTOCOL_I2C;
        }
    }
    
    // Call the new centralized entry function
    bootloader_entry(protocol);
    
    return 0;
}