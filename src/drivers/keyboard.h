#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <system.h>
#include "../gui/gui.h"

// controller ports
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

// special keys
#define KEY_ENTER 0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_SPACE 0x39
#define KEY_ESC 0x01
#define KEY_TAB 0x0F
#define KEY_CTRL 0x1D
#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEY_LEFT 0x4B
#define KEY_RIGHT 0x4D

// key state tracking
extern int ctrl_pressed;

extern void init_keyboard(void);
extern char scancode_to_ascii(uint8_t scancode);
extern char get_key(void);
extern uint8_t get_scancode(void);
extern void handle_keyboard_input(void);
extern char convert_scancode_to_layout(uint8_t scancode, KeyboardLayout layout);

#endif
