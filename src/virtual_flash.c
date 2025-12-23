#include "virtual_flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLASH_SIZE 0x20000  // 128KB

static uint8_t virtual_flash[FLASH_SIZE];

void virtual_flash_init(void) {
    // Initialize with erased state (0xFF)
    memset(virtual_flash, 0xFF, FLASH_SIZE);
    
    // Create a simple boot header at the beginning
    virtual_flash[0] = 0x08;  // Some magic bytes
    virtual_flash[1] = 0xAA;
    virtual_flash[2] = 0x55;
    virtual_flash[3] = 0x08;
    
    printf("[FLASH] Initialized %d KB virtual flash\n", FLASH_SIZE / 1024);
}

uint8_t virtual_flash_read(uint32_t address) {
    if (address >= FLASH_SIZE) {
        printf("[FLASH ERROR] Read address out of bounds: 0x%08X\n", address);
        return 0xFF;
    }
    return virtual_flash[address];
}

uint32_t virtual_flash_read_word(uint32_t address) {
    if (address + 3 >= FLASH_SIZE) {
        return 0xFFFFFFFF;
    }
    
    return (virtual_flash[address]) |
           (virtual_flash[address + 1] << 8) |
           (virtual_flash[address + 2] << 16) |
           (virtual_flash[address + 3] << 24);
}

void virtual_flash_write(uint32_t address, uint8_t *data, uint32_t length) {
    if (address + length > FLASH_SIZE) {
        printf("[FLASH ERROR] Write address out of bounds\n");
        return;
    }
    
    // Flash can only be written from 1->0 (in simulation we just write)
    for(uint32_t i = 0; i < length; i++) {
        // Simulate flash programming: can only clear bits
        virtual_flash[address + i] &= data[i];
    }
    
    printf("[FLASH] Written %d bytes at 0x%08X\n", length, address);
}

void virtual_flash_erase_page(uint32_t address) {
    uint32_t page_start = address - (address % 1024);
    
    if (page_start + 1024 > FLASH_SIZE) {
        printf("[FLASH ERROR] Erase address out of bounds\n");
        return;
    }
    
    // Erase page (set all bytes to 0xFF)
    memset(&virtual_flash[page_start], 0xFF, 1024);
    printf("[FLASH] Page at 0x%08X erased\n", page_start);
}

void virtual_flash_dump(uint32_t start, uint32_t length) {
    if (start + length > FLASH_SIZE) {
        printf("[FLASH ERROR] Dump range out of bounds\n");
        return;
    }
    
    printf("Flash dump from 0x%08X to 0x%08X:\n", start, start + length);
    for(uint32_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\n0x%08X: ", start + i);
        }
        printf("%02X ", virtual_flash[start + i]);
    }
    printf("\n");
}

bool virtual_flash_verify(uint32_t address, uint8_t *data, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        if (virtual_flash_read(address + i) != data[i]) {
            return false;
        }
    }
    return true;
}