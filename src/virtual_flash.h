#ifndef VIRTUAL_FLASH_H
#define VIRTUAL_FLASH_H

#include <stdint.h>
#include <stdbool.h>

void virtual_flash_init(void);
uint8_t virtual_flash_read(uint32_t address);
uint32_t virtual_flash_read_word(uint32_t address);
void virtual_flash_write(uint32_t address, uint8_t *data, uint32_t length);
void virtual_flash_erase_page(uint32_t address);
void virtual_flash_dump(uint32_t start, uint32_t length);
bool virtual_flash_verify(uint32_t address, uint8_t *data, uint32_t length);

#endif // VIRTUAL_FLASH_H