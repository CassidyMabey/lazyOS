#include "keyboard.h"
#include "vga.h"
#include <stdint.h>
#include <system.h>

// global state
int ctrl_pressed = 0;

void init_keyboard(void) {
    // ready evne tfor keyboard
    while (inb(KBD_STATUS_PORT) & 2);
    // turn on keyboard
    outb(KBD_DATA_PORT, 0xF4);
}

char scancode_to_ascii(uint8_t scancode) {
    static const char ascii_table[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
        0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
        '*', 0, ' '
    };
    
    if (scancode < sizeof(ascii_table)) {
        return ascii_table[scancode];
    }
    return 0;
}

char get_key(void) {
    if (inb(KBD_STATUS_PORT) & 1) { 
        uint8_t scancode = inb(KBD_DATA_PORT);
        if (!(scancode & 0x80)) {  // check press
            // convert press into the key
            return convert_scancode_to_layout(scancode, get_current_layout());
        }
    }
    return 0; 
}

uint8_t get_scancode(void) {
    if (inb(KBD_STATUS_PORT) & 1) { 
        return inb(KBD_DATA_PORT);
    }
    return 0;
}

void handle_keyboard_input(void) {
    uint8_t scancode = get_scancode();
    if (scancode == 0) return;
    
    int key_released = scancode & 0x80;
    scancode &= 0x7F; // Remove release bit
    
    // Handle Ctrl key
    if (scancode == KEY_CTRL) {
        ctrl_pressed = !key_released;
        return;
    }
    
    // Handle Ctrl + Arrow key combinations for window movement
    if (ctrl_pressed && !key_released) {
        extern Window* window_list;
        extern void draw_desktop(void);
        extern void redraw_windows(void);
        
        // Find the topmost window (assuming it's the focused one)
        Window* focused_window = window_list;
        if (!focused_window || focused_window->is_minimized || focused_window->is_maximized) {
            return; // No window to move or window is maximized/minimized
        }
        
        int move_step = 5; // Pixels to move per key press
        int moved = 0;
        
        switch (scancode) {
            case KEY_UP:
                if (focused_window->y > move_step) {
                    focused_window->y -= move_step;
                    moved = 1;
                }
                break;
            case KEY_DOWN:
                if (focused_window->y + focused_window->height + move_step < VGA_HEIGHT - 1) {
                    focused_window->y += move_step;
                    moved = 1;
                }
                break;
            case KEY_LEFT:
                if (focused_window->x > move_step) {
                    focused_window->x -= move_step;
                    moved = 1;
                }
                break;
            case KEY_RIGHT:
                if (focused_window->x + focused_window->width + move_step < VGA_WIDTH) {
                    focused_window->x += move_step;
                    moved = 1;
                }
                break;
        }
        
        if (moved) {
            draw_desktop();
            redraw_windows();
        }
    }
}

char convert_scancode_to_layout(uint8_t scancode, KeyboardLayout layout) {
    // QWERTY layout is the base layout
    if (layout == LAYOUT_QWERTY) {
        return scancode_to_ascii(scancode);
    }

    // convert pressed key into key
    char qwerty_char = scancode_to_ascii(scancode);
    
    switch (layout) {
        case LAYOUT_DVORAK: {
            static const char qwerty[] = "qwertyuiop[]asdfghjkl;'zxcvbnm,./";
            static const char dvorak[] = "',.pyfgcrl/=aoeuidhtns-;qjkxbmwvz";
            
            // lower case
            for (int i = 0; qwerty[i]; i++) {
                if (qwerty_char == qwerty[i]) {
                    return dvorak[i];
                }
            }
            break;
        }
        
        case LAYOUT_AZERTY: {
            static const char qwerty[] = "qwertyuiop[]asdfghjkl;'zxcvbnm,./1234567890";
            static const char azerty[] = "azertyuiop^$qsdfghjklm%*wxcvbn,;:!&é\"'(-è_ç";
            
            // lower case & numbers
            for (int i = 0; qwerty[i]; i++) {
                if (qwerty_char == qwerty[i]) {
                    return azerty[i];
                }
            }
            break;
        }
        
        default:
            break;
    }
    
    // just in case
    return qwerty_char;
}
