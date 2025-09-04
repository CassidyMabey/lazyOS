#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "../memory/memory.h"

// layout
typedef enum {
    LAYOUT_QWERTY,
    LAYOUT_DVORAK,
    LAYOUT_AZERTY
} KeyboardLayout;

#define MAX_WINDOWS 16
#define TITLE_BAR_HEIGHT 1

// window structure
typedef struct Window {
    int x, y;
    int width, height;
    const char* title;
    uint8_t color;
    char* content;
    int is_settings;
    int is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int z_index;         // stacking
    struct Window* next; 
} Window;

void init_gui(void);
void draw_desktop(void);
Window* create_window(int x, int y, int width, int height, const char* title);
void draw_window(int x, int y, int width, int height, const char* title);
void redraw_windows(void);
void handle_input(char key);
void toggle_settings_sidebar(void);
void draw_settings_sidebar(void);
KeyboardLayout get_current_layout(void);
void set_keyboard_layout(KeyboardLayout layout);

void handle_mouse_input(void);
void check_window_interaction(int mouse_x, int mouse_y, uint8_t buttons);
void bring_window_to_front(Window* win);

#endif
