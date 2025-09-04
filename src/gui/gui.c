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

// global menu state
int menu_hover_index = -1;
int menu_visible = 0;

// show menu with window options
void show_menu_options(int x, int y) {
    menu_visible = 1;
    const char* options[] = {
        "New Window",
        "Close All",
        "About"
    };
    
    // draw menu box
    for (int i = 0; i < 3; i++) {
        uint8_t bg_color = (i == menu_hover_index) ? VGA_BLUE : VGA_BLACK;
        uint8_t fg_color = (i == menu_hover_index) ? VGA_WHITE : VGA_LIGHT_GREY;
        
        // bg for each line
        for (int j = 0; j < 15; j++) {
            putchar_at(' ', (bg_color << 4) | fg_color, x + j, y + i);
        }
        // show text
        const char* opt = options[i];
        for (int j = 0; opt[j]; j++) {
            putchar_at(opt[j], (bg_color << 4) | fg_color, x + 1 + j, y + i);
        }
    }
}

// handle menu clicks
void handle_menu_click(int mouse_x, int mouse_y) {
    // check if clicking menu area
    if (mouse_x < 15 && mouse_y >= VGA_HEIGHT - 4 && mouse_y < VGA_HEIGHT - 1) {
        int option = mouse_y - (VGA_HEIGHT - 4);
        switch (option) {
            case 0: // new window
                create_window(10 + (window_count * 5), 3 + (window_count * 2), 35, 12, "Window");
                break;
            case 1: // close all
                while (window_list) {
                    remove_window(window_list);
                }
                break;
            case 2: // about
                create_window(25, 8, 30, 8, "About LazyOS");
                break;
        }
        draw_desktop();
        redraw_windows();
    }
}

// take window off list
// update about window  
static void draw_about_content(Window* win) {
    if (!win || strcmp(win->title, "About LazyOS") != 0) return;
    
    const char* lines[] = {
        "LazyOS v1.0",
        "Custom OS",
        "Made in C"
    };
    
    // show info
    int y = win->y + 2;
    for (int i = 0; i < 3; i++) {
        const char* line = lines[i];
        for (int x = 0; line[x]; x++) {
            putchar_at(line[x], VGA_WHITE, win->x + 2 + x, y + i);
        }
    }
}

// update settings window
static void draw_settings_content(Window* win) {
    if (!win || strcmp(win->title, "Settings") != 0) return;
    
    // draw layout options
    const char* layouts[] = {
        "qwerty",
        "dvorak", 
        "azerty"
    };
    
    KeyboardLayout current = get_current_layout();
    
    // show options
    int y = win->y + 2;
    for (int i = 0; i < 3; i++) {
        const char* layout = layouts[i];
        // show > if picked
        char marker = ((KeyboardLayout)i == current) ? '>' : ' ';
        putchar_at(marker, VGA_WHITE, win->x + 2, y + i);
        // show name
        for (int x = 0; layout[x]; x++) {
            putchar_at(layout[x], VGA_WHITE, win->x + 4 + x, y + i);
        }
    }
    
    // show help text
    const char* help = "press space to pick";
    int y_help = y + 5;
    for (int x = 0; help[x]; x++) {
        putchar_at(help[x], VGA_WHITE, win->x + 2 + x, y_help);
    }
}

void init_gui(void) {
    init_vga();
    init_mouse();
    draw_desktop();
}

void redraw_windows(void) {
    // draw all windows
    for (int z = 0; z < window_count; z++) {
        Window* current = window_list;
        while (current) {
            if (current->z_index == z) {
                // Skip minimized windows in main drawing
                if (!current->is_minimized) {
                    int is_hovered = (current == hovered_window) ? 1 : 0;
                    draw_window_with_hover(current->x, current->y, current->width, current->height, current->title, is_hovered);
                    // show window stuff
                    if (strcmp(current->title, "Settings") == 0) {
                        draw_settings_content(current);
                    } else if (strcmp(current->title, "About LazyOS") == 0) {
                        draw_about_content(current);
                    }
                }
                break;
            }
            current = current->next;
        }
    }
    
    // Draw minimized window titles in taskbar
    int taskbar_pos = 20; // Start after menu and settings buttons
    Window* current = window_list;
    while (current) {
        if (current->is_minimized) {
            // Draw minimized window title in taskbar
            const char* title = current->title;
            int title_len = 0;
            while (title[title_len] && taskbar_pos + title_len < VGA_WIDTH - 1) {
                putchar_at(title[title_len], VGA_WHITE, taskbar_pos + title_len, VGA_HEIGHT - 1);
                title_len++;
            }
            taskbar_pos += title_len + 2; // Add spacing between titles
        }
        current = current->next;
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

// handle keys in window
static void handle_window_input(Window* win, char key) {
    if (!win) return;
    
    if (strcmp(win->title, "Settings") == 0) {
        // settings keys
        if (key == ' ') {
            // change layout
            KeyboardLayout current = get_current_layout();
            if (current < LAYOUT_AZERTY) {
                set_keyboard_layout(current + 1);
            } else {
                set_keyboard_layout(LAYOUT_QWERTY);
            }
            // show new pick
            draw_settings_content(win);
        }
    } else if (strcmp(win->title, "Command") == 0) {
        // cmd input
        if (key == '\n') {
            // new line
        } else if (key >= 32 && key <= 126) {
            // type char
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
    // taskbar bg
    for (int i = 0; i < VGA_WIDTH; i++) {
        putchar_at(' ', (VGA_DARK_GREY << 4) | VGA_WHITE, i, VGA_HEIGHT - 1);
    }
    
    // menu button
    const char* menu_text = " Menu ";
    for (int i = 0; menu_text[i]; i++) {
        putchar_at(menu_text[i], (VGA_DARK_GREY << 4) | VGA_LIGHT_CYAN, i, VGA_HEIGHT - 1);
    }
    
    // settings button next to menu
    const char* set_text = " Settings ";
    int set_x = strlen(menu_text);
    for (int i = 0; set_text[i]; i++) {
        putchar_at(set_text[i], (VGA_DARK_GREY << 4) | VGA_LIGHT_GREEN, set_x + i, VGA_HEIGHT - 1);
    }
    
    // system info
    const char* sys_info = "LazyOS v1.0";
    int info_x = VGA_WIDTH - strlen(sys_info) - 1;
    for (int i = 0; sys_info[i]; i++) {
        putchar_at(sys_info[i], (VGA_DARK_GREY << 4) | VGA_LIGHT_GREY, info_x + i, VGA_HEIGHT - 1);
    }
}

void draw_desktop(void) {
    // First, clear the entire screen to ensure no artifacts
    clear_screen();
    
    // gradient bg  
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        uint8_t bg_color = (y < VGA_HEIGHT/2) ? VGA_BLUE : VGA_LIGHT_BLUE;
        for (int x = 0; x < VGA_WIDTH; x++) {
            putchar_at(BOX_SHADE, (bg_color << 4) | VGA_WHITE, x, y);
        }
    }
    
    // taskbar
    draw_taskbar();
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
    
    // set window stuff
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
