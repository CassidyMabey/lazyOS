#include "vga.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_BUFFER;

void init_vga(void) {
    clear_screen();
}

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (VGA_LIGHT_GREY << 8) | ' ';
    }
}

void putchar_at(char c, uint8_t color, int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        const int index = y * VGA_WIDTH + x;
        vga_buffer[index] = (color << 8) | c;
    }
}

void draw_box(int x1, int y1, int x2, int y2, uint8_t color) {
    // Draw horizontal lines
    for (int x = x1; x <= x2; x++) {
        putchar_at('-', color, x, y1);
        putchar_at('-', color, x, y2);
    }
    
    // Draw vertical lines
    for (int y = y1; y <= y2; y++) {
        putchar_at('|', color, x1, y);
        putchar_at('|', color, x2, y);
    }
    
    // Draw corners
    putchar_at('+', color, x1, y1);
    putchar_at('+', color, x2, y1);
    putchar_at('+', color, x1, y2);
    putchar_at('+', color, x2, y2);
}

void draw_window(int x, int y, int width, int height, const char* title) {
    // Draw window border
    draw_box(x, y, x + width - 1, y + height - 1, VGA_WHITE);
    
    // Draw title
    int title_x = x + (width - strlen(title)) / 2;
    for (int i = 0; title[i] != '\0'; i++) {
        putchar_at(title[i], VGA_WHITE, title_x + i, y);
    }
}
