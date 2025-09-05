#include "keyboard.h"
#include "vga.h"
#include "mouse.h"
#include <stdint.h>
#include <system.h>

// global state
int ctrl_pressed = 0;
char current_key = 0;
int keys_pressed = 0;
KeyboardLayout current_layout = LAYOUT_QWERTY;

// Local keyboard state tracking
static uint8_t shift_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t caps_lock = 0;
static uint8_t num_lock = 0;

void init_keyboard(void) {
    // wait for keyboard controller
    while (inb(KBD_STATUS_PORT) & 0x02);
    
    // disable keyboard interrupts
    outb(KBD_DATA_PORT, 0xF5); 
    

    while (!(inb(KBD_STATUS_PORT) & 0x01));
    inb(KBD_DATA_PORT);
    
    // clear keyboard buffer
    while (inb(KBD_STATUS_PORT) & 0x01) {
        inb(KBD_DATA_PORT);
    }
    
    // renable keyboard no interrupts
    while (inb(KBD_STATUS_PORT) & 0x02);
    outb(KBD_DATA_PORT, 0xF4); 

    while (!(inb(KBD_STATUS_PORT) & 0x01));
    inb(KBD_DATA_PORT); 
    

    // debug
    outb(0x3F8, 'K');
    outb(0x3F8, 'B');
    outb(0x3F8, 'D');
    outb(0x3F8, ' ');
    outb(0x3F8, 'O');
    outb(0x3F8, 'K');
    outb(0x3F8, '\n');
}

char scancode_to_ascii(uint8_t scancode) {
    // scancode table
    static const char ascii_table[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    if (scancode < 128) {
        return ascii_table[scancode];
    }
    return 0;
}

char get_key(void) {
    uint8_t status = inb(KBD_STATUS_PORT);
    
    // check keyboard data
    if (!(status & 0x01)) return 0;
    
    if (status & 0x20) {
        return 0;
    }
    
    uint8_t scancode = inb(KBD_DATA_PORT);
    
    // debug
    outb(0x3F8, 'K');
    outb(0x3F8, ':');  
    outb(0x3F8, '0' + (scancode / 100));
    outb(0x3F8, '0' + ((scancode / 10) % 10));
    outb(0x3F8, '0' + (scancode % 10));
    outb(0x3F8, ' ');
    
    if (!(scancode & 0x80)) {  

        switch(scancode) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                shift_pressed = 1;
                outb(0x3F8, 'S'); // shift
                outb(0x3F8, 'H');
                outb(0x3F8, ' ');
                return 0;
            case KEY_CTRL:
                ctrl_pressed = 1;
                outb(0x3F8, 'C');  // ctrl
                outb(0x3F8, 'T');
                outb(0x3F8, ' ');
                return 0;
            case KEY_CAPSLOCK:
                caps_lock = !caps_lock;
                outb(0x3F8, 'C');  // caps
                outb(0x3F8, 'P');
                outb(0x3F8, ' ');
                return 0;
        }
        
        char ascii = scancode_to_ascii(scancode);
        if (ascii) {
            // apply shift/caps
            if ((shift_pressed ^ caps_lock) && ascii >= 'a' && ascii <= 'z') {
                ascii = ascii - 'a' + 'A';
            }
            
            // debug
            outb(0x3F8, 'A'); 
            outb(0x3F8, ':');
            outb(0x3F8, ascii >= 32 && ascii <= 126 ? ascii : '?');
            outb(0x3F8, ' ');
            
            return ascii;
        }
    } else {
        // key release
        scancode &= 0x7F;
        switch(scancode) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                shift_pressed = 0;
                break;
            case KEY_CTRL:
                ctrl_pressed = 0;
                break;
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

void handle_keyboard_input_internal(uint8_t scancode) {
    int key_released = scancode & 0x80;
    scancode &= 0x7F; // release bit removed
    switch(scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift_pressed = !key_released;
            break;
            
        case KEY_CTRL:
            ctrl_pressed = !key_released;
            break;
            
        case KEY_ALT:
            alt_pressed = !key_released;
            break;
            
        case KEY_CAPSLOCK:
            if (!key_released) caps_lock = !caps_lock;
            break;
            
        case KEY_NUMLOCK:
            if (!key_released) num_lock = !num_lock;
            break;
            
        default:
            if (!key_released) {
                extern void draw_desktop(void);
                extern void redraw_windows(void);
            }
            break;
    }
}

void handle_keyboard_input(void) {
    if (!(inb(KBD_STATUS_PORT) & 0x01)) {
        return;
    }
    
    uint8_t scancode = inb(KBD_DATA_PORT);
    if (scancode == 0xE0) {
        // TODO add some more keys which arent in ASCII table 
        return;
    }
    
    int key_released = scancode & 0x80;
    scancode &= 0x7F; // release bit removed
    
    // othe rkeys
    switch(scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift_pressed = !key_released;
            break;
            
        case KEY_CTRL:
            ctrl_pressed = !key_released;
            break;
            
        case KEY_ALT:
            alt_pressed = !key_released;
            break;
            
        case KEY_CAPSLOCK:
            if (!key_released) caps_lock = !caps_lock;
            break;
            
        case KEY_NUMLOCK:
            if (!key_released) num_lock = !num_lock;
            break;
            
        default:
            if (!key_released) {
                extern void draw_desktop(void);
                extern void redraw_windows(void);
                
            }
            break;
    }
}

char convert_scancode_to_layout(uint8_t scancode, KeyboardLayout layout) {
    // use QWERTY as base
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
