#include "vga.h"
#include "../gui/gui_constants.h"
#include "../gui/gui.h"
#include <string.h>

static volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_BUFFER;

// screen size
int VGA_WIDTH = 80;   // default width
int VGA_HEIGHT = 25;  // default height

// talk to vga card
static inline void write_crtc(uint8_t index, uint8_t value) {
    outb(VGA_CRTC_INDEX, index);
    outb(VGA_CRTC_DATA, value);
}

static inline uint8_t read_crtc(uint8_t index) {
    outb(VGA_CRTC_INDEX, index);
    return inb(VGA_CRTC_DATA);
}

void init_vga(void) {
    // get screen size
    uint8_t h_end = read_crtc(0x01) + 1;  // width
    uint8_t v_end = read_crtc(0x12) + 1;  // height
    
    // set size if ok
    if (h_end >= 40 && h_end <= 200) VGA_WIDTH = h_end;
    if (v_end >= 25 && v_end <= 60) VGA_HEIGHT = v_end;
    
    // wait for display
    while (inb(0x3DA) & 0x8);
    
    outb(VGA_AC_INDEX, 0x30);
    outb(VGA_AC_WRITE, 0x00);
    
    // setup colors
    for (int i = 0; i < 16; i++) {
        outb(0x3C8, i);
        // enhance colors
        outb(0x3C9, (i & 4) ? ((i & 8) ? 0x3F : 0x2A) : 0);
        outb(0x3C9, (i & 2) ? ((i & 8) ? 0x3F : 0x2A) : 0);
        outb(0x3C9, (i & 1) ? ((i & 8) ? 0x3F : 0x2A) : 0);
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
    for (int x = x1 + 1; x < x2; x++) {
        putchar_at(BOX_HLINE, color, x, y1); 
        putchar_at(BOX_HLINE, color, x, y2); 
    }
    
    for (int y = y1 + 1; y < y2; y++) {
        putchar_at(BOX_VLINE, color, x1, y); 
        putchar_at(BOX_VLINE, color, x2, y); 
    }
    
    putchar_at(BOX_TLCORNER, color, x1, y1); 
    putchar_at(BOX_TRCORNER, color, x2, y1); 
    putchar_at(BOX_BLCORNER, color, x1, y2); 
    putchar_at(BOX_BRCORNER, color, x2, y2); 
}

void draw_window(int x, int y, int width, int height, const char* title) {
    draw_window_with_hover(x, y, width, height, title, 0);
}

void draw_window_with_hover(int x, int y, int width, int height, const char* title, int is_hovered) {
    // draw window frame with double lines on top
    for (int i = x + 1; i < x + width - 1; i++) {
        putchar_at(BOX_DHLINE, VGA_WHITE, i, y);  // top border double
        putchar_at(BOX_HLINE, VGA_WHITE, i, y + height - 1);  // bottom border single
    }
    for (int i = y + 1; i < y + height - 1; i++) {
        putchar_at(BOX_DVLINE, VGA_WHITE, x, i);  // left border double
        putchar_at(BOX_VLINE, VGA_WHITE, x + width - 1, i);  // right border single
    }
    
    // draw corners
    putchar_at(BOX_DTLCORNER, VGA_WHITE, x, y);  // top left double
    putchar_at(BOX_DTRCORNER, VGA_WHITE, x + width - 1, y);  // top right double
    putchar_at(BOX_BLCORNER, VGA_WHITE, x, y + height - 1);  // bottom left single
    putchar_at(BOX_BRCORNER, VGA_WHITE, x + width - 1, y + height - 1);  // bottom right single
    
    // title bar colors for hover effect
    uint8_t title_bg = is_hovered ? VGA_BLUE : VGA_BLACK;
    uint8_t title_fg = is_hovered ? VGA_YELLOW : VGA_WHITE;

    // Draw window control buttons on the right side of title bar
    uint8_t button_color = VGA_WHITE;
    // Close button (X)
    putchar_at('x', button_color, x + width - 2, y);
    // Maximize button (□)
    putchar_at(0xFE, button_color, x + width - 4, y);  // Using a square character
    // Minimize button (_)
    putchar_at('_', button_color, x + width - 6, y);
    
    for (int i = x + 1; i < x + width - 1; i++) {
        putchar_at(' ', (title_bg << 4) | title_fg, i, y);
    }
    
    // Draw title centered in title bar
    int title_x = x + (width - strlen(title)) / 2;
    putchar_at(' ', (title_bg << 4) | title_fg, title_x - 1, y);  // space before title
    putchar_at(CHAR_BULLET, (title_bg << 4) | title_fg, title_x - 1, y);  // bullet point before title
    for (int i = 0; title[i] != '\0'; i++) {
        putchar_at(title[i], (title_bg << 4) | title_fg, title_x + i, y);
    }
    putchar_at(' ', (title_bg << 4) | title_fg, title_x + strlen(title), y);  // space after title
    
    // Fill window content area with background
    for (int wy = y + 1; wy < y + height - 1; wy++) {
        for (int wx = x + 1; wx < x + width - 1; wx++) {
            putchar_at(' ', (VGA_BLACK << 4) | VGA_WHITE, wx, wy);
        }
    }
}
