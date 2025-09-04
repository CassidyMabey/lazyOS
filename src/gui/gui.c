#include "gui.h"
#include "gui_constants.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/mouse.h"
#include "../util/string.h"

Window* window_list = NULL;
static Window* active_window = NULL;
Window* dragging_window = NULL;  // mouse needs this
Window* hovered_window = NULL;   // mouse needs this
static int window_count = 0;
IconType active_icon = ICON_TYPE_NONE;  // no icon picked

// take window off list
static void remove_window(Window* win) {
    if (!win) return;
    
    Window** current = &window_list;
    while (*current) {
        if (*current == win) {
            *current = win->next;
            free(win);
            window_count--;
            draw_desktop();
            redraw_windows();
            break;
        }
        current = &(*current)->next;
    }
}

void init_gui(void) {
    init_vga();
    init_mouse();
    draw_desktop();
}

void redraw_windows(void) {
    // keep everything showing after the first frame
    for (int z = 0; z < window_count; z++) {
        Window* current = window_list;
        while (current) {
            if (current->z_index == z) {
                int is_hovered = (current == hovered_window) ? 1 : 0;
                draw_window_with_hover(current->x, current->y, current->width, current->height, current->title, is_hovered);
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

// handle keyboard input for window content
static void handle_window_input(Window* win, char key) {
    if (!win || !win->content) return;
    
    if (strcmp(win->title, "Command") == 0) {
        // handle cmd input
        if (key == '\n') {
            // new line
        } else if (key >= 32 && key <= 126) {
            // printable chars
        }
    }
}

void draw_taskbar_icon(int x, IconType type, int is_active) {
    // icon colors
    uint8_t bg = is_active ? VGA_BLUE : VGA_BLACK;
    uint8_t fg = is_active ? VGA_YELLOW : VGA_WHITE;
    uint8_t color = (bg << 4) | fg;
    
    // draw icon box
    for (int i = 0; i < ICON_WIDTH; i++) {
        putchar_at(' ', color, x + i, TASKBAR_Y);
    }
    
    // icon symbol
    char icon_char = ' ';
    const char* icon_text = "";
    
    switch (type) {
        case ICON_TYPE_NONE:
            // no icon
            break;
        case ICON_TYPE_FILES:
            icon_char = ICON_FILES;
            icon_text = "files";
            break;
        case ICON_TYPE_CMD:
            icon_char = ICON_CMD;
            icon_text = "cmd";
            break;
        case ICON_TYPE_SETTINGS:
            icon_char = ICON_SETTINGS;
            icon_text = "cfg";
            break;
        case ICON_TYPE_COUNT:
            // ignore count case
            break;
    }
    
    // draw icon and text
    putchar_at(icon_char, color, x + 1, TASKBAR_Y);
    for (int i = 0; icon_text[i] && i < ICON_WIDTH - 3; i++) {
        putchar_at(icon_text[i], color, x + 3 + i, TASKBAR_Y);
    }
}

void draw_taskbar(void) {
    // draw taskbar bg
    for (int i = 0; i < VGA_WIDTH; i++) {
        putchar_at(' ', VGA_BLACK << 4, i, TASKBAR_Y);
    }
    
    // draw icons
    int x = 0;
    for (IconType i = ICON_TYPE_FILES; i < ICON_TYPE_COUNT; i++) {
        draw_taskbar_icon(x, i, i == active_icon);
        x += ICON_WIDTH;
    }
}

void draw_desktop(void) {
    // gradient bg
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        uint8_t bg_color = (y < VGA_HEIGHT/2) ? VGA_BLUE : VGA_LIGHT_BLUE;
        for (int x = 0; x < VGA_WIDTH; x++) {
            putchar_at(BOX_SHADE, (bg_color << 4) | VGA_WHITE, x, y);
        }
    }
    
    for (int x = 0; x < VGA_WIDTH; x++) {
        putchar_at(' ', (VGA_DARK_GREY << 4) | VGA_WHITE, x, VGA_HEIGHT - 1);
    }
    
    const char* menu = " Menu "; 
    putchar_at(BOX_TLCORNER, (VGA_DARK_GREY << 4) | VGA_WHITE, 0, VGA_HEIGHT - 1);
    putchar_at(BOX_TRCORNER, (VGA_DARK_GREY << 4) | VGA_WHITE, MENU_BUTTON_WIDTH - 1, VGA_HEIGHT - 1);
    for (int i = 0; menu[i]; i++) {
        putchar_at(menu[i], (VGA_DARK_GREY << 4) | VGA_LIGHT_CYAN, i + 1, VGA_HEIGHT - 1);
    }
    
    const char* sys_info = "LazyOS v1.0";
    int info_x = VGA_WIDTH - strlen(sys_info) - 1;
    for (int i = 0; sys_info[i]; i++) {
        putchar_at(sys_info[i], (VGA_DARK_GREY << 4) | VGA_LIGHT_GREY, info_x + i, VGA_HEIGHT - 1);
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
    
    // set content based on type
    if (strcmp(title, "Files") == 0) {
        win->content = "file browser";
    } else if (strcmp(title, "Command") == 0) {
        win->content = "cmd prompt";
    } else if (strcmp(title, "Settings") == 0) {
        win->content = "system settings";
    } else {
        win->content = NULL;
    }
    
    win->next = window_list;
    window_list = win;
    
    // active windows
    active_window = win;
    
    draw_window(x, y, width, height, title);
    return win;
}

void handle_input(char key) {
    // handle input for active window
    if (active_window) {
        handle_window_input(active_window, key);
        draw_desktop();
        redraw_windows();
        return;
    }
    
    // no window active check special keys
    uint8_t scancode = (uint8_t)key;
    if (scancode == KEY_ESC) {
        // close active icon
        if (active_icon != ICON_TYPE_NONE) {
            active_icon = ICON_TYPE_NONE;
            draw_desktop();
            redraw_windows();
        }
    }
}
