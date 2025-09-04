#include "gui.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/mouse.h"
#include <string.h>

static Window* window_list = NULL;
static Window* active_window = NULL;
static Window* dragging_window = NULL;
static int settings_visible = 0;
static int window_count = 0;

void init_gui(void) {
    init_vga();
    init_mouse();
    draw_desktop();
    if (settings_visible) {
        draw_settings_sidebar();
    }
}

void redraw_windows(void) {
    // keep everything showing after the first frame
    for (int z = 0; z < window_count; z++) {
        Window* current = window_list;
        while (current) {
            if (current->z_index == z) {
                draw_window(current->x, current->y, current->width, current->height, current->title);
                break;
            }
            current = current->next;
        }
    }
}

void bring_window_to_front(Window* win) {
    if (!win || win == active_window) return;

    Window* current = window_list;
    while (current) {
        if (current->z_index > win->z_index) {
            current->z_index--;
        }
        current = current->next;
    }
    win->z_index = window_count - 1;
    active_window = win;
    
    draw_desktop();
    redraw_windows();
}

void draw_desktop(void) {
    // background
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            putchar_at(' ', (VGA_BLUE << 4) | VGA_WHITE, x, y);
        }
    }
    
    // taskbar
    for (int x = 0; x < VGA_WIDTH; x++) {
        putchar_at(' ', (VGA_LIGHT_GREY << 4) | VGA_BLACK, x, VGA_HEIGHT - 1);
    }
    
    // menu
    const char* menu = "[ Menu ]";
    for (int i = 0; menu[i]; i++) {
        putchar_at(menu[i], (VGA_LIGHT_GREY << 4) | VGA_BLACK, i, VGA_HEIGHT - 1);
    }
}

static void redraw_windows(void) {
    // redraw
    for (int z = 0; z < window_count; z++) {
        Window* current = window_list;
        while (current) {
            if (current->z_index == z) {
                draw_window(current->x, current->y, current->width, current->height, current->title);
                break;
            }
            current = current->next;
        }
    }
}

Window* create_window(int x, int y, int width, int height, const char* title) {
    if (window_count >= MAX_WINDOWS) return NULL;
    
    Window* win = (Window*)malloc(sizeof(Window));
    if (!win) return NULL;

    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->title = title;
    win->color = VGA_WHITE;
    win->is_dragging = 0;
    win->z_index = window_count++;
    win->next = window_list;
    window_list = win;
    
    // active windows
    active_window = win;
    
    draw_window(x, y, width, height, title);
    return win;
}

void handle_input(char key) {
    // special keys handling
    uint8_t scancode = (uint8_t)key;
    
    if (scancode == KEY_ESC) {
        toggle_settings_sidebar();
        draw_desktop();
        if (settings_visible) {
            draw_settings_sidebar();
        }
    } else if (settings_visible) {
        // settings menu
        if (scancode == KEY_ENTER) {
            KeyboardLayout current = get_current_layout();
            if (current < LAYOUT_AZERTY) {
                set_keyboard_layout((KeyboardLayout)(current + 1));
            } else {
                set_keyboard_layout(LAYOUT_QWERTY);
            }
            draw_settings_sidebar();
        }
    }
}
