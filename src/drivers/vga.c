#include "vga.h"
#include <string.h>

static volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_BUFFER;

void init_vga(void) {
    // wait until display
    while (inb(0x3DA) & 0x8);
    
    // text mode & blinking off
    outb(0x3C0, 0x30); outb(0x3C0, 0x00);
    
    // background to black 
    for (int i = 0; i < 16; i++) {
        outb(0x3C8, i);
        outb(0x3C9, (i & 4) ? 0x3F : 0);
        outb(0x3C9, (i & 2) ? 0x3F : 0);
        outb(0x3C9, (i & 1) ? 0x3F : 0);
    }
    
    clear_screen();
}

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (VGA_LIGHT_GREY << 8) | ' ';
    }
}

// mouse position
static int cursor_x = 0;
static int cursor_y = 0;

void putchar_at(char c, uint8_t color, int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        const int index = y * VGA_WIDTH + x;
        vga_buffer[index] = (color << 8) | c;
    }
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

    putchar_at(c, VGA_WHITE, cursor_x, cursor_y);
    
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            cursor_y = 0;
        }
    }
}

void draw_box(int x1, int y1, int x2, int y2, uint8_t color) {
    for (int x = x1; x <= x2; x++) {
        putchar_at('-', color, x, y1);
        putchar_at('-', color, x, y2);
    }
    
    for (int y = y1; y <= y2; y++) {
        putchar_at('|', color, x1, y);
        putchar_at('|', color, x2, y);
    }
    
    putchar_at('+', color, x1, y1);
    putchar_at('+', color, x2, y1);
    putchar_at('+', color, x1, y2);
    putchar_at('+', color, x2, y2);
}

void draw_window(int x, int y, int width, int height, const char* title) {
    // border
    draw_box(x, y, x + width - 1, y + height - 1, VGA_WHITE);
    
    // title
    int title_x = x + (width - strlen(title)) / 2;
    for (int i = 0; title[i] != '\0'; i++) {
        putchar_at(title[i], VGA_WHITE, title_x + i, y);
    }
}
