#ifndef GUI_H
#define GUI_H

#include <stdint.h>

// Keyboard layout enum
typedef enum {
    LAYOUT_QWERTY,
    LAYOUT_DVORAK,
    LAYOUT_AZERTY
} KeyboardLayout;

// Window structure
typedef struct {
    int x, y;
    int width, height;
    const char* title;
    uint8_t color;
    char* content;
    int is_settings;
} Window;

void init_gui(void);
Window* create_window(int x, int y, int width, int height, const char* title);
void draw_desktop(void);
void handle_input(char key);
void toggle_settings_sidebar(void);
void draw_settings_sidebar(void);
KeyboardLayout get_current_layout(void);
void set_keyboard_layout(KeyboardLayout layout);

#endif
