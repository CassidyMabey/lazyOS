#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <system.h>

// buffer
#define VGA_BUFFER 0xB8000

// VGA ports for getting screen size
#define VGA_AC_INDEX      0x3C0
#define VGA_AC_WRITE      0x3C0
#define VGA_AC_READ       0x3C1
#define VGA_MISC_WRITE    0x3C2
#define VGA_SEQ_INDEX     0x3C4
#define VGA_SEQ_DATA      0x3C5
#define VGA_GC_INDEX      0x3CE
#define VGA_GC_DATA       0x3CF
#define VGA_CRTC_INDEX    0x3D4
#define VGA_CRTC_DATA     0x3D5

extern int VGA_WIDTH;
extern int VGA_HEIGHT;

// colors
#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_YELLOW 14
#define VGA_WHITE 15
#define VGA_MAGENTA 5
#define VGA_BROWN 6
#define VGA_LIGHT_GREY 7
#define VGA_DARK_GREY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_BROWN 14
#define VGA_YELLOW 14
#define VGA_WHITE 15

extern void init_vga(void);
extern void clear_screen(void);
extern void putchar_at(char c, uint8_t color, int x, int y);
extern void putchar(char c); 
extern void draw_box(int x1, int y1, int x2, int y2, uint8_t color);
extern void draw_window(int x, int y, int width, int height, const char* title);
extern void draw_window_with_hover(int x, int y, int width, int height, const char* title, int is_hovered);
extern void draw_window(int x, int y, int width, int height, const char* title);
extern void draw_window_with_hover(int x, int y, int width, int height, const char* title, int is_hovered);

#endif
