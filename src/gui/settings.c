#include "gui.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include <string.h>

#define SETTINGS_WIDTH 20
#define SETTINGS_HEIGHT 10
#define SETTINGS_X (VGA_WIDTH - SETTINGS_WIDTH)
#define SETTINGS_Y 0

static int settings_visible = 0;

// layout names
static const char* const layout_names[] = {
    "QWERTY",
    "DVORAK",
    "AZERTY"
};

void draw_settings_sidebar(void) {
    // draw settings
    for (int y = SETTINGS_Y; y < SETTINGS_Y + SETTINGS_HEIGHT; y++) {
        for (int x = SETTINGS_X; x < VGA_WIDTH; x++) {
            putchar_at(' ', (VGA_LIGHT_GREY << 4) | VGA_BLACK, x, y);
        }
    }

    // title
    const char* title = "Settings";
    int title_x = SETTINGS_X + (SETTINGS_WIDTH - strlen(title)) / 2;
    for (int i = 0; title[i]; i++) {
        putchar_at(title[i], (VGA_LIGHT_GREY << 4) | VGA_BLACK, title_x + i, SETTINGS_Y);
    }

    // keyboard layout
    const char* section = "Keyboard Layout:";
    for (int i = 0; section[i]; i++) {
        putchar_at(section[i], (VGA_LIGHT_GREY << 4) | VGA_BLACK, SETTINGS_X + 1, SETTINGS_Y + 2);
    }

    // layout options
    for (size_t i = 0; i < sizeof(layout_names) / sizeof(layout_names[0]); i++) {
        char marker = (get_current_layout() == (KeyboardLayout)i) ? '>' : ' ';
        putchar_at(marker, (VGA_LIGHT_GREY << 4) | VGA_BLACK, SETTINGS_X + 2, SETTINGS_Y + 4 + i);
        
        const char* layout = layout_names[i];
        for (int j = 0; layout[j]; j++) {
            putchar_at(layout[j], (VGA_LIGHT_GREY << 4) | VGA_BLACK, SETTINGS_X + 4 + j, SETTINGS_Y + 4 + i);
        }
    }
}

void toggle_settings_sidebar(void) {
    settings_visible = !settings_visible;
    if (settings_visible) {
        draw_settings_sidebar();
    } else {
        draw_desktop();
    }
}

KeyboardLayout get_current_layout(void) {
    return current_layout;
}

void set_keyboard_layout(KeyboardLayout layout) {
    if (layout >= LAYOUT_QWERTY && layout <= LAYOUT_AZERTY) {
        current_layout = layout;
        // draw the sidebar
        if (settings_visible) {
            draw_settings_sidebar();
        }
    }
}
