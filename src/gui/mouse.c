void handle_mouse_input(void) {
    MouseState mouse = get_mouse_state();
    check_window_interaction(mouse.x, mouse.y, mouse.buttons);
}

void check_window_interaction(int mouse_x, int mouse_y, uint8_t buttons) {
    static int last_click_x = 0;
    static int last_click_y = 0;
    
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

    // Check for new window interactions
    if (buttons & MOUSE_LEFT_BTN) {
        Window* current = window_list;
        while (current) {
            // Check if click is in title bar
            if (mouse_y == current->y && 
                mouse_x >= current->x && 
                mouse_x < current->x + current->width) {
                
                // Start dragging
                current->is_dragging = 1;
                current->drag_offset_x = mouse_x - current->x;
                current->drag_offset_y = mouse_y - current->y;
                dragging_window = current;
                bring_window_to_front(current);
                break;
            }
            
            // Check if click is inside window
            if (mouse_x >= current->x && 
                mouse_x < current->x + current->width &&
                mouse_y >= current->y && 
                mouse_y < current->y + current->height) {
                
                bring_window_to_front(current);
                break;
            }
            
            current = current->next;
        }
    }
    
    // Store last click position
    last_click_x = mouse_x;
    last_click_y = mouse_y;
}
