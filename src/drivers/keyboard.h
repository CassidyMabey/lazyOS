#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "../gui/gui.h"

// Keyboard controller ports
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

// Special keys
#define KEY_ENTER 0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_SPACE 0x39
#define KEY_ESC 0x01
#define KEY_TAB 0x0F

uint8_t inb(uint16_t port);
char scancode_to_ascii(uint8_t scancode);
void init_keyboard(void);
char get_key(void);
char convert_scancode_to_layout(uint8_t scancode, KeyboardLayout layout);

#endif
