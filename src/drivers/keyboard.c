#include "keyboard.h"
#include <stdint.h>
#include <system.h>

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
