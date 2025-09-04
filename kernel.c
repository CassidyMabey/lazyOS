#include <stdint.h>
#include "multiboot.h"

__attribute__((section(".multiboot"))) 
const multiboot_header mb_header = {
    .magic = MULTIBOOT_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
};


#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x07    


#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64


#define KEY_ENTER 0x1C
#define KEY_BACKSPACE 0x0E


static uint16_t* vga_buffer = (uint16_t*)VGA_BUFFER;
static int cursor_x = 0;
static int cursor_y = 0;


static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}


void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (VGA_COLOR << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}


void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            cursor_y = 0;
        }
        return;
    }

    int index = cursor_y * VGA_WIDTH + cursor_x;
    vga_buffer[index] = (VGA_COLOR << 8) | c;
    
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            cursor_y = 0;
        }
    }
}


void handle_backspace() {
    if (cursor_x > 0) {
        cursor_x--;
        int index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = (VGA_COLOR << 8) | ' ';
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = VGA_WIDTH - 1;
        int index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = (VGA_COLOR << 8) | ' ';
    }
}


char scancode_to_ascii(uint8_t scancode) {
    
    if (scancode >= 0x10 && scancode <= 0x19) {  
        return 'Q' + (scancode - 0x10);
    }
    if (scancode >= 0x1E && scancode <= 0x26) {  
        return 'A' + (scancode - 0x1E);
    }
    if (scancode >= 0x2C && scancode <= 0x32) {  
        return 'Z' + (scancode - 0x2C);
    }
    return 0;  
}

void kernel_main(void) {
    clear_screen();

    while (1) {
        
        if (inb(KBD_STATUS_PORT) & 1) {
            uint8_t scancode = inb(KBD_DATA_PORT);
            
            
            if (!(scancode & 0x80)) {
                if (scancode == KEY_ENTER) {
                    putchar('\n');
                } else if (scancode == KEY_BACKSPACE) {
                    clear_screen();  
                } else {
                    char c = scancode_to_ascii(scancode);
                    if (c != 0) {
                        putchar(c);
                    }
                }
            }
        }
    }
}
