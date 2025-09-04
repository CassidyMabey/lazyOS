#include "gui.h"
#include "../drivers/vga.h"
#include <string.h>

#define MAX_WINDOWS 4
static Window windows[MAX_WINDOWS];
static int window_count = 0;
static int settings_visible = 0;
static KeyboardLayout current_layout = LAYOUT_QWERTY;

void init_gui(void) {
    init_vga();
    draw_desktop();
    if (settings_visible) {
        draw_settings_sidebar();
    }
}

void draw_desktop(void) {
    // Clear screen with blue background
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            putchar_at(' ', (VGA_BLUE << 4) | VGA_WHITE, x, y);
        }
    }
    
    // Draw taskbar
    for (int x = 0; x < VGA_WIDTH; x++) {
        putchar_at(' ', (VGA_LIGHT_GREY << 4) | VGA_BLACK, x, VGA_HEIGHT - 1);
    }
    
    // Draw system menu
    const char* menu = "[ Menu ]";
    for (int i = 0; menu[i]; i++) {
        putchar_at(menu[i], (VGA_LIGHT_GREY << 4) | VGA_BLACK, i, VGA_HEIGHT - 1);
    }
}

Window* create_window(int x, int y, int width, int height, const char* title) {
    if (window_count >= MAX_WINDOWS) return NULL;
    
    Window* win = &windows[window_count++];
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->title = title;
    win->color = VGA_WHITE;
    
    draw_window(x, y, width, height, title);
    return win;
}

void handle_input(char key) {
    // TODO: Implement window focus and input handling
}
