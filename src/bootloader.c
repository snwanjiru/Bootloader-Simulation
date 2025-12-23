#include <sys/select.h>
#include "bootloader.h"
#include "virtual_flash.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static BootloaderStatus status = {0};
static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];
static uint32_t rx_index = 0;

// Professional additions
#define BOOTLOADER_MAGIC_NUMBER 0xDEADBEEF
#define BOOTLOADER_TIMEOUT_SEC  5  // 5 second timeout
#define MIN_BOOTLOADER_AREA     0x1000  // Protect first 4KB

// Platform-independent timing for simulation
static time_t bootloader_start_time = 0;

// Helper function to check if timeout has occurred
static bool bootloader_check_timeout(void) {
    time_t current_time;
    time(&current_time);
    return (current_time - bootloader_start_time) >= BOOTLOADER_TIMEOUT_SEC;
}

// Simple XOR checksum for simulation
static uint8_t calculate_checksum(uint8_t *data, uint32_t length) {
    uint8_t checksum = 0;
    for(uint32_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void bootloader_init(ProtocolType protocol) {
    printf("[BOOT] Initializing bootloader...\n");
    printf("[BOOT] Version: 0x%04X\n", BOOTLOADER_VERSION);
    
    status.active_protocol = protocol;
    status.initialized = true;
    status.write_protected = false;
    status.readout_protected = false;
    
    // Set boot start time for timeout
    time(&bootloader_start_time);
    
    // Initialize virtual flash
    virtual_flash_init();
    
    printf("[BOOT] Ready on protocol: %d\n", protocol);
    printf("[BOOT] Will auto-jump to app in %d seconds if no command\n", 
           BOOTLOADER_TIMEOUT_SEC);
}

// NEW FUNCTION: Enhanced application validation
bool bootloader_check_app_valid(uint32_t app_address) {
    // Check if application reset vector is valid
    uint32_t stack_pointer = virtual_flash_read_word(app_address);
    uint32_t reset_vector = virtual_flash_read_word(app_address + 4);
    uint32_t magic_number = virtual_flash_read_word(app_address + 8);
    
    printf("[VERIFY] SP: 0x%08X, Reset: 0x%08X, Magic: 0x%08X\n", 
           stack_pointer, reset_vector, magic_number);
    
    // Professional validation checks:
    // 1. Check stack pointer is reasonable (not erased value)
    if (stack_pointer == 0xFFFFFFFF || stack_pointer == 0x00000000) {
        printf("[VERIFY] Invalid stack pointer\n");
        return false;
    }
    
    // 2. Check reset vector is within flash bounds
    if (reset_vector == 0xFFFFFFFF || reset_vector < app_address || 
        reset_vector >= (app_address + FLASH_SIZE)) {
        printf("[VERIFY] Invalid reset vector\n");
        return false;
    }
    
    // 3. Check magic number (optional but good practice)
    if (magic_number != BOOTLOADER_MAGIC_NUMBER) {
        printf("[VERIFY] Warning: Magic number mismatch (expected 0x%08X, got 0x%08X)\n",
               BOOTLOADER_MAGIC_NUMBER, magic_number);
        // Don't fail on magic number for compatibility
    }
    
    // 4. Add CRC check here in real implementation
    // status.app_crc = bootloader_calculate_crc(app_address, app_end_address);
    
    return true;
}

// NEW FUNCTION: Bootloader main entry with timeout
void bootloader_entry(ProtocolType protocol) {
    bootloader_init(protocol);
    
    printf("[BOOT] Press any key within %d seconds for command mode...\n",
           BOOTLOADER_TIMEOUT_SEC);
    
    // Wait for commands with timeout
    while(1) {
        // Check for command input (non-blocking simulation)
        printf(".");
        fflush(stdout);
        
        // In real hardware, this would check UART/SPI/I2C buffer
        // For simulation, we'll use a simple timeout approach
        if (bootloader_check_timeout()) {
            printf("\n[BOOT] Timeout reached\n");
            // Try to jump to valid application
            if (bootloader_check_app_valid(APP_START_ADDRESS)) {
                printf("[BOOT] Auto-jumping to valid application\n");
                bootloader_jump_to_app();
                return;
            } else {
                printf("[BOOT] No valid application found. Staying in bootloader.\n");
                // Stay in bootloader without timeout
                break;
            }
        }
        
        // Check if user wants to enter command mode
        #ifdef _WIN32
        // Windows-specific (would use _kbhit() in real code)
        #else
        // Linux/Mac: simple simulation
        struct timeval tv = {0, 100000}; // 100ms timeout
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        if (select(1, &fds, NULL, NULL, &tv) > 0) {
            // User pressed a key, enter command mode
            printf("\n");
            break;
        }
        #endif
    }
    
    // Enter interactive command mode
    printf("[BOOT] Entering command mode\n");
    while(1) {
        if (!bootloader_process_command()) {
            // Command handler requested jump or quit
            break;
        }
    }
}

// Enhanced command processor
bool bootloader_process_command(void) {
    static bool in_bootloader = true;
    
    if (!in_bootloader) {
        return false;
    }
    
    printf("\n[BOOT] Waiting for command...\n");
    printf("Available commands:\n");
    printf("  i - Get bootloader info\n");
    printf("  r - Read memory\n");
    printf("  w - Write memory\n");
    printf("  e - Erase flash\n");
    printf("  g - Jump to application\n");
    printf("  v - Verify application (enhanced)\n");
    printf("  s - Set magic number for app\n");
    printf("  p - Print memory map\n");
    printf("  q - Quit simulator\n");
    
    char cmd;
    scanf(" %c", &cmd);
    
    switch(cmd) {
        case 'i': {
            printf("[INFO] Bootloader v%d.%d\n", 
                   (BOOTLOADER_VERSION >> 8) & 0xFF,
                   BOOTLOADER_VERSION & 0xFF);
            printf("[INFO] Flash size: %d KB\n", FLASH_SIZE / 1024);
            printf("[INFO] Protocol: %d\n", status.active_protocol);
            printf("[INFO] Boot area: 0x%08X - 0x%08X\n", 
                   BOOTLOADER_ADDRESS, MIN_BOOTLOADER_AREA - 1);
            printf("[INFO] App area:   0x%08X - 0x%08X\n", 
                   APP_START_ADDRESS, FLASH_SIZE);
            break;
        }
        
        case 'r': {
            uint32_t address, length;
            printf("Enter address (hex): ");
            scanf("%x", &address);
            printf("Enter length: ");
            scanf("%d", &length);
            
            if (address + length > FLASH_SIZE) {
                printf("[ERROR] Address out of range\n");
                break;
            }
            
            printf("Data at 0x%08X:\n", address);
            for(uint32_t i = 0; i < length; i++) {
                if (i % 16 == 0) printf("\n0x%08X: ", address + i);
                uint8_t data = virtual_flash_read(address + i);
                printf("%02X ", data);
            }
            printf("\n");
            break;
        }
        
        case 'w': {
            if (status.write_protected) {
                printf("[ERROR] Write protected\n");
                break;
            }
            
            uint32_t address;
            printf("Enter address (hex): ");
            scanf("%x", &address);
            
            // Professional: Protect bootloader area
            if (address < MIN_BOOTLOADER_AREA) {
                printf("[ERROR] Cannot write to bootloader area (0x0000-0x%04X)\n", 
                       MIN_BOOTLOADER_AREA - 1);
                printf("Use APP_START_ADDRESS: 0x%08X\n", APP_START_ADDRESS);
                break;
            }
            
            printf("Enter data (hex bytes, space separated, end with -1):\n");
            uint32_t index = 0;
            int value;
            
            while(index < sizeof(rx_buffer)) {
                if (scanf("%x", &value) != 1 || value == -1) break;
                rx_buffer[index++] = value & 0xFF;
            }
            
            virtual_flash_write(address, rx_buffer, index);
            printf("[OK] Written %d bytes to 0x%08X\n", index, address);
            break;
        }
        
        case 'e': {
            uint32_t page;
            printf("Enter page number to erase (0-%d): ", 
                   FLASH_SIZE / FLASH_PAGE_SIZE - 1);
            scanf("%d", &page);
            
            // Prevent erasing bootloader area
            uint32_t page_address = page * FLASH_PAGE_SIZE;
            if (page_address < MIN_BOOTLOADER_AREA) {
                printf("[ERROR] Cannot erase bootloader area\n");
                printf("Bootloader pages: 0-%d\n", 
                       (MIN_BOOTLOADER_AREA / FLASH_PAGE_SIZE) - 1);
                break;
            }
            
            virtual_flash_erase_page(page_address);
            printf("[OK] Page %d erased\n", page);
            break;
        }
        
        case 'g': {
            if (bootloader_check_app_valid(APP_START_ADDRESS)) {
                printf("[BOOT] Jumping to application at 0x%08X\n", 
                       APP_START_ADDRESS);
                in_bootloader = false;
                bootloader_jump_to_app();
                return false;
            } else {
                printf("[ERROR] Application verification failed\n");
            }
            break;
        }
        
        case 'v': {
            // Use enhanced validation
            if (bootloader_check_app_valid(APP_START_ADDRESS)) {
                printf("[OK] Application verified and ready to run\n");
            } else {
                printf("[ERROR] Application verification failed\n");
            }
            break;
        }
        
        case 's': {
            // Helper command to set magic number for testing
            uint32_t magic = BOOTLOADER_MAGIC_NUMBER;
            virtual_flash_write(APP_START_ADDRESS + 8, (uint8_t*)&magic, 4);
            printf("[OK] Magic number 0x%08X written at 0x%08X\n",
                   magic, APP_START_ADDRESS + 8);
            break;
        }
        
        case 'p': {
            printf("=== Memory Map ===\n");
            printf("Bootloader: 0x%08X - 0x%08X (%d bytes)\n",
                   BOOTLOADER_ADDRESS, MIN_BOOTLOADER_AREA - 1,
                   MIN_BOOTLOADER_AREA - BOOTLOADER_ADDRESS);
            printf("Reserved:   0x%08X - 0x%08X (%d bytes)\n",
                   MIN_BOOTLOADER_AREA, APP_START_ADDRESS - 1,
                   APP_START_ADDRESS - MIN_BOOTLOADER_AREA);
            printf("Application: 0x%08X - 0x%08X (%d KB)\n",
                   APP_START_ADDRESS, FLASH_SIZE,
                   (FLASH_SIZE - APP_START_ADDRESS) / 1024);
            break;
        }
        
        case 'q': {
            printf("[BOOT] Exiting simulator\n");
            return false;
        }
        
        default:
            printf("[ERROR] Unknown command\n");
    }
    
    return true;
}

// Keep original for backward compatibility
bool bootloader_verify_app(void) {
    return bootloader_check_app_valid(APP_START_ADDRESS);
}

// Enhanced jump function
void bootloader_jump_to_app(void) {
    printf("[BOOT] =========================================\n");
    printf("[BOOT] Jumping to application at 0x%08X\n", APP_START_ADDRESS);
    printf("[BOOT] Simulation: Application mode activated\n");
    printf("[BOOT] =========================================\n");
    
    // In real hardware, this would:
    // 1. Disable interrupts
    // 2. Set vector table offset
    // 3. Set main stack pointer from first word of app
    // 4. Jump to reset vector (second word of app)
    
    // For simulation, we'll show what would happen:
    uint32_t stack_pointer = virtual_flash_read_word(APP_START_ADDRESS);
    uint32_t reset_vector = virtual_flash_read_word(APP_START_ADDRESS + 4);
    
    printf("[APP] Stack Pointer: 0x%08X\n", stack_pointer);
    printf("[APP] Reset Vector:  0x%08X\n", reset_vector);
    printf("[APP] Application running...\n");
    printf("[APP] Press Enter to return to bootloader...\n");
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); // Wait for Enter
    
    printf("[APP] Returning to bootloader...\n");
}

// NEW: CRC calculation function (stub for real implementation)
uint32_t bootloader_calculate_crc(uint32_t start, uint32_t end) {
    printf("[CRC] CRC calculation would run from 0x%08X to 0x%08X\n", start, end);
    // In real implementation, compute CRC-32 or similar
    return 0x12345678; // Placeholder
}