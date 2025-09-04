#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <system.h>

// mouse port
#define MOUSE_DATA_PORT    0x60
#define MOUSE_STATUS_PORT  0x64
#define MOUSE_CMD_PORT     0x64

// mouse commands
#define MOUSE_WRITE       0xD4
#define MOUSE_READ        0x20
#define MOUSE_ENABLE      0xA8
#define MOUSE_DEFAULTS    0xF6
#define MOUSE_ENABLE_PKT  0xF4

// mouse buttons & flags
#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x04
#define MOUSE_X_SIGN      0x10
#define MOUSE_Y_SIGN      0x20
#define MOUSE_X_OVERFLOW  0x40
#define MOUSE_Y_OVERFLOW  0x80

typedef struct {
    int x;
    int y;
    uint8_t buttons;
} MouseState;

extern void init_mouse(void);
extern void handle_mouse_packet(void);
extern MouseState get_mouse_state(void);

#endif
