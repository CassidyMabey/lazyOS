#include "mouse.h"
#include "../drivers/mouse.h"
#include "../drivers/vga.h"
#include "gui.h"
#include "gui_constants.h"
#include <string.h>

// get windows from gui
extern Window* window_list;

// gui functions we need
extern void show_menu_options(int x, int y);
extern void handle_menu_click(int mouse_x, int mouse_y);
extern Window* create_window(int x, int y, int width, int height, const char* title);
extern void draw_desktop(void);
extern void redraw_windows(void);

void handle_mouse_input(void) {
    MouseState mouse = get_mouse_state();
    
    // Store previous menu state
    int prev_menu_hover = menu_hover_index;
    
    // Reset menu hover state
    menu_hover_index = -1;
    
    // Check if mouse is in menu area and menu is visible
    if (menu_visible && 
        mouse.x >= 0 && mouse.x < 15 && 
        mouse.y >= VGA_HEIGHT - 4 && mouse.y < VGA_HEIGHT - 1) {
        
        // Calculate which menu item is being hovered
        menu_hover_index = mouse.y - (VGA_HEIGHT - 4);
        
        // Redraw menu if hover state changed
        if (prev_menu_hover != menu_hover_index) {
            show_menu_options(0, VGA_HEIGHT - 4);
        }
    }
    
    check_window_interaction(mouse.x, mouse.y, mouse.buttons);
}

void check_taskbar_click(int mouse_x, int mouse_y, uint8_t buttons) {
    if (mouse_y != VGA_HEIGHT - 1) {
        // If clicking outside menu area and menu is visible, hide it
        if (menu_visible && 
            (mouse_y < VGA_HEIGHT - 4 || mouse_y >= VGA_HEIGHT - 1 ||
             mouse_x < 0 || mouse_x >= 15)) {
            menu_visible = 0;
            draw_desktop();
            redraw_windows();
        }
        return;
    }
    
    if (buttons & MOUSE_LEFT_BTN) {
        // check menu button (0-6)
        if (mouse_x >= 0 && mouse_x < 6) {
            // Toggle menu visibility
            menu_visible = !menu_visible;
            if (menu_visible) {
                show_menu_options(0, VGA_HEIGHT - 4);
            } else {
                draw_desktop();
                redraw_windows();
            }
            return;
        }
        
        // check settings button (6-16) 
        if (mouse_x >= 6 && mouse_x < 16) {
            // make settings window
            create_window(30, 5, 25, 10, "Settings");
            draw_desktop();
            redraw_windows();
            return;
        }
        
        // check minimized windows (after pos 20)
        if (mouse_x >= 20) {
            int pos = 20;
            Window* current = window_list;
            while (current) {
                if (current->is_minimized) {
                    int title_len = strlen(current->title);
                    if (mouse_x >= pos && mouse_x < pos + title_len) {
                        // Restore window
                        current->is_minimized = 0;
                        bring_window_to_front(current);
                        draw_desktop();
                        redraw_windows();
                        return;
                    }
                    pos += title_len + 2;
                }
                current = current->next;
            }
        }
    }
}

void handle_window_controls(Window* window, int mouse_x, int mouse_y) {
    // Only handle controls if clicking in title bar
    if (mouse_y != window->y) return;
    
    // Check close button (x)
    if (mouse_x == window->x + window->width - 2) {
        // Remove window from list
        if (window == window_list) {
            window_list = window->next;
        } else {
            Window* prev = window_list;
            while (prev && prev->next != window) {
                prev = prev->next;
            }
            if (prev) {
                prev->next = window->next;
            }
        }
        free(window);
        draw_desktop();
        redraw_windows();
        return;
    }
    
    // Check maximize button (□)
    if (mouse_x == window->x + window->width - 4) {
        if (!window->is_maximized) {
            // Store original dimensions
            window->orig_x = window->x;
            window->orig_y = window->y;
            window->orig_width = window->width;
            window->orig_height = window->height;
            // Maximize
            window->x = 0;
            window->y = 0;
            window->width = VGA_WIDTH;
            window->height = VGA_HEIGHT - 1; // Leave space for taskbar
        } else {
            // Restore original dimensions
            window->x = window->orig_x;
            window->y = window->orig_y;
            window->width = window->orig_width;
            window->height = window->orig_height;
        }
        window->is_maximized = !window->is_maximized;
        draw_desktop();
        redraw_windows();
        return;
    }
    
    // Check minimize button (_)
    if (mouse_x == window->x + window->width - 6) {
        window->is_minimized = !window->is_minimized;
        draw_desktop();
        redraw_windows();
        return;
    }
}

void check_window_interaction(int mouse_x, int mouse_y, uint8_t buttons) {
    // check taskbar first
    check_taskbar_click(mouse_x, mouse_y, buttons);
    
    // check menu options if shown
    if (buttons & MOUSE_LEFT_BTN) {
        handle_menu_click(mouse_x, mouse_y);
    }
    
    // track hovering and handle clicks
    Window* prev_hovered = hovered_window;
    hovered_window = NULL;
    
    // Find topmost window under cursor
    Window* current = window_list;
    Window* clicked_window = NULL;
    
    while (current) {
        if (!current->is_minimized &&
            mouse_x >= current->x && 
            mouse_x < current->x + current->width &&
            mouse_y >= current->y && 
            mouse_y < current->y + current->height) {
            
            // Found a window under cursor
            if (!clicked_window) {
                clicked_window = current;
            }
            hovered_window = current;
        }
        current = current->next;
    }
    
    // Handle click on window
    if (buttons & MOUSE_LEFT_BTN && clicked_window) {
        // Check for window control buttons first
        handle_window_controls(clicked_window, mouse_x, mouse_y);
    }
    
    // redraw if hover changed
    if (prev_hovered != hovered_window) {
        draw_desktop();
        redraw_windows();
    }

    // dragging
    if (dragging_window) {
        if (buttons & MOUSE_LEFT_BTN) {
            // smoother dragging
            int new_x = mouse_x - dragging_window->drag_offset_x;
            int new_y = mouse_y - dragging_window->drag_offset_y;
            
            // fix window positions
            if (new_x < 0) new_x = 0;
            if (new_y < 0) new_y = 0;
            if (new_x + dragging_window->width > VGA_WIDTH) 
                new_x = VGA_WIDTH - dragging_window->width;
            if (new_y + dragging_window->height > VGA_HEIGHT - 1)
                new_y = VGA_HEIGHT - 1 - dragging_window->height;
            
            // window pos
            if (new_x != dragging_window->x || new_y != dragging_window->y) {
                dragging_window->x = new_x;
                dragging_window->y = new_y;
                draw_desktop();
                redraw_windows();
            }
        } else {
            // stop dragging
            dragging_window->is_dragging = 0;
            dragging_window = NULL;
        }
        return;
    }

    // check for interaction with the window
    if (buttons & MOUSE_LEFT_BTN) {
        Window* current = window_list;
        Window* clicked_window = NULL;
        
        // Find the topmost window that was clicked
        while (current) {
            if (!current->is_minimized &&
                mouse_x >= current->x && 
                mouse_x < current->x + current->width &&
                mouse_y >= current->y && 
                mouse_y < current->y + current->height) {
                clicked_window = current;
                break; // First window found is the topmost
            }
            current = current->next;
        }
        
        if (clicked_window) {
            // Bring window to front
            bring_window_to_front(clicked_window);
            
            // Check if clicking on title bar for dragging
            if (mouse_y == clicked_window->y) {
                // Check for window control buttons first
                int control_handled = 0;
                
                // Check close button (x)
                if (mouse_x == clicked_window->x + clicked_window->width - 2) {
                    handle_window_controls(clicked_window, mouse_x, mouse_y);
                    control_handled = 1;
                }
                // Check maximize button (□)
                else if (mouse_x == clicked_window->x + clicked_window->width - 4) {
                    handle_window_controls(clicked_window, mouse_x, mouse_y);
                    control_handled = 1;
                }
                // Check minimize button (_)
                else if (mouse_x == clicked_window->x + clicked_window->width - 6) {
                    handle_window_controls(clicked_window, mouse_x, mouse_y);
                    control_handled = 1;
                }
                
                // If no control button was clicked, start dragging
                if (!control_handled) {
                    clicked_window->is_dragging = 1;
                    clicked_window->drag_offset_x = mouse_x - clicked_window->x;
                    clicked_window->drag_offset_y = mouse_y - clicked_window->y;
                    dragging_window = clicked_window;
                }
            }
            
            draw_desktop();
            redraw_windows();
        }
    }
}
