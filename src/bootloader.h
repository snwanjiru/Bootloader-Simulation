#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

// Bootloader configuration
#define BOOTLOADER_VERSION    0x0100
#define FLASH_SIZE            0x20000  // 128KB
#define FLASH_PAGE_SIZE       1024
#define APP_START_ADDRESS     0x08004000
#define BOOTLOADER_ADDRESS    0x08000000

// Command codes
typedef enum {
    CMD_GET_INFO      = 0x00,
    CMD_READ_MEMORY   = 0x11,
    CMD_WRITE_MEMORY  = 0x31,
    CMD_ERASE         = 0x43,
    CMD_GO            = 0x21,
    CMD_WRITE_PROTECT = 0x63,
    CMD_WRITE_UNPROTECT = 0x73,
    CMD_READOUT_PROTECT = 0x82,
    CMD_READOUT_UNPROTECT = 0x92
} BootloaderCommand;

// Protocol types
typedef enum {
    PROTOCOL_UART,
    PROTOCOL_SPI,
    PROTOCOL_I2C
} ProtocolType;

// Bootloader status
typedef struct {
    bool initialized;
    bool write_protected;
    bool readout_protected;
    ProtocolType active_protocol;
    uint32_t app_crc;
} BootloaderStatus;

// Function prototypes
void bootloader_init(ProtocolType protocol);
void bootloader_entry(ProtocolType protocol);
bool bootloader_process_command(void);
void bootloader_jump_to_app(void);
bool bootloader_verify_app(void);
uint32_t bootloader_calculate_crc(uint32_t start, uint32_t end);

#endif // BOOTLOADER_H