#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "../memory/memory.h"

#define MAX_WINDOWS 16
#define TITLE_BAR_HEIGHT 1
#define MENU_BUTTON_WIDTH 8
#define MENU_WINDOW_WIDTH 30
#define MENU_WINDOW_HEIGHT 15

typedef struct Window Window;

// window structure
struct Window {
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
};

// global window state
extern Window* window_list;
extern Window* dragging_window;
extern Window* hovered_window;

// layout
// taskbar icons
typedef enum IconType {
    ICON_TYPE_NONE = -1,  // no icon selected
    ICON_TYPE_FILES = 0,
    ICON_TYPE_CMD,
    ICON_TYPE_SETTINGS,
    ICON_TYPE_COUNT
} IconType;

// active icon
extern IconType active_icon;

typedef enum {
    LAYOUT_QWERTY,
    LAYOUT_DVORAK,
    LAYOUT_AZERTY
} KeyboardLayout;

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
void create_menu_window(void);
void close_menu_window(void);

#endif
