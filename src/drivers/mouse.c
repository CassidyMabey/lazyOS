#include "mouse.h"
#include "vga.h"

static MouseState mouse_state = {0, 0, 0};
static uint8_t mouse_cycle = 0;
static uint8_t mouse_packet[3];

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 1) == 1) {
                return;
            }
        }
    } else {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 2) == 0) {
                return;
            }
        }
    }
}

static void mouse_write(uint8_t write) {
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, MOUSE_WRITE);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, write);
}

static uint8_t mouse_read(void) {
    mouse_wait(0);
    return inb(MOUSE_DATA_PORT);
}

void init_mouse(void) {
    uint8_t status;

    // mouse device
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, MOUSE_ENABLE);

    // interruptes
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, MOUSE_READ);
    mouse_wait(0);
    status = inb(MOUSE_DATA_PORT) | 2;
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    // streaming
    mouse_write(MOUSE_DEFAULTS);
    mouse_write(MOUSE_ENABLE_PKT);

    // mouse pos
    mouse_state.x = VGA_WIDTH / 2;
    mouse_state.y = VGA_HEIGHT / 2;
}

void handle_mouse_packet(void) {
    uint8_t data = mouse_read();

    switch(mouse_cycle) {
        case 0:
            if ((data & 0x08) == 0x08) {
                mouse_packet[0] = data;
                mouse_cycle++;
            }
            break;
        case 1:
            mouse_packet[1] = data;
            mouse_cycle++;
            break;
        case 2:
            mouse_packet[2] = data;
            mouse_cycle = 0;

            // mouse state
            mouse_state.buttons = mouse_packet[0] & 0x07;

            // x
            int x_mov = mouse_packet[1];
            if (mouse_packet[0] & MOUSE_X_SIGN) {
                x_mov |= 0xFFFFFF00;
            }
            
            // y 
            int y_mov = mouse_packet[2];
            if (mouse_packet[0] & MOUSE_Y_SIGN) {
                y_mov |= 0xFFFFFF00;
            }
            y_mov = -y_mov; 

            // update pos with bounds checking and increased sensitivity
            mouse_state.x += x_mov * 2;  // Doubled sensitivity for wider screen
            mouse_state.y += y_mov * 2;  // Doubled sensitivity for taller screen


            // stop from escaping from the screen (need to change if multiple moniters.)
            if (mouse_state.x < 0) mouse_state.x = 0;
            if (mouse_state.y < 0) mouse_state.y = 0;
            if (mouse_state.x >= VGA_WIDTH) mouse_state.x = VGA_WIDTH - 1;
            if (mouse_state.y >= VGA_HEIGHT) mouse_state.y = VGA_HEIGHT - 1;
            break;
    }
}

MouseState get_mouse_state(void) {
    return mouse_state;
}
