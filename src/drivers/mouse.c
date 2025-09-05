#include "mouse.h"
#include "vga.h"

static MouseState mouse_state = {0, 0, 0};

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
    while ((inb(MOUSE_STATUS_PORT) & 2) != 0) { }
    
    // aux mouse device enable
    outb(MOUSE_CMD_PORT, 0xA8);
    
    // enable interrupts
    outb(MOUSE_CMD_PORT, 0x20);
    uint8_t status = inb(MOUSE_DATA_PORT) | 2;
    outb(MOUSE_CMD_PORT, 0x60);
    outb(MOUSE_DATA_PORT, status);
    
    // default settintgs
    mouse_write(0xF6);
    if (mouse_read() != 0xFA) { 
        mouse_write(0xFF);
        mouse_read();
        mouse_read(); 
        mouse_read(); 
    }
    
    // write streaming
    mouse_write(0xF4);
    mouse_read(); 
    
    // wheel and mouse buttons
    mouse_write(0xF3); 
    mouse_read();
    mouse_write(200); // sample rate 200
    mouse_read(); 
    mouse_write(0xF3);
    mouse_read();
    mouse_write(100);
    mouse_read();
    mouse_write(0xF3);
    mouse_read();
    mouse_write(80);
    mouse_read();
    
    // mouse ID
    mouse_write(0xF2);
    mouse_read();
    uint8_t mouse_id = mouse_read();
    (void) mouse_id;
    
    // set polling rate and resolution
    mouse_write(0xE8);
    mouse_read();
    mouse_write(0x03);
    mouse_read();
    
    // set scaling to default
    mouse_write(0xE6);
    mouse_read();
    
    // data reporting enable
    mouse_write(0xF4);
    mouse_read();
    
    // default mouse position
    mouse_state.x = VGA_WIDTH / 2;
    mouse_state.y = VGA_HEIGHT / 2;
    mouse_state.buttons = 0;
    
    show_mouse_cursor();
}

void handle_mouse_packet(void) {
    uint8_t status = inb(MOUSE_STATUS_PORT);
    
    if (!(status & 0x01)) {
        return;
    }
    
    if (!(status & 0x20)) {
        return;
    }
    
    static uint8_t cycle = 0;
    static uint8_t packet[4];
    
    // readn a byte of data from the mouse
    uint8_t data = inb(MOUSE_DATA_PORT);
    
    switch(cycle) {
        case 0:
            // looking for start of packet
            if ((data & 0x08) == 0x08 && (data & 0xC0) == 0) {
                packet[0] = data;
                cycle++;
            }
            break;
            
        case 1:
            // x
            packet[1] = data;
            cycle++;
            break;
            
        case 2:
            // y
            packet[2] = data;
                        
            uint8_t old_buttons = mouse_state.buttons;
            mouse_state.buttons = packet[0] & 0x07;
            
            if ((mouse_state.buttons & MOUSE_LEFT_BTN) && !(old_buttons & MOUSE_LEFT_BTN)) {
                // lmb pressed
                outb(0x3F8, 'L'); outb(0x3F8, 'D'); outb(0x3F8, 'N'); outb(0x3F8, '\n');
            }
            if (!(mouse_state.buttons & MOUSE_LEFT_BTN) && (old_buttons & MOUSE_LEFT_BTN)) {
                // lmb released
                outb(0x3F8, 'L'); outb(0x3F8, 'U'); outb(0x3F8, 'P'); outb(0x3F8, '\n');
            }
            if ((mouse_state.buttons & MOUSE_RIGHT_BTN) && !(old_buttons & MOUSE_RIGHT_BTN)) {
                // rmb pressed
                outb(0x3F8, 'R'); outb(0x3F8, 'D'); outb(0x3F8, 'N'); outb(0x3F8, '\n');
            }
            if (!(mouse_state.buttons & MOUSE_RIGHT_BTN) && (old_buttons & MOUSE_RIGHT_BTN)) {
                // rmb released
                outb(0x3F8, 'R'); outb(0x3F8, 'U'); outb(0x3F8, 'P'); outb(0x3F8, '\n');
            }
            
            // get x and y 
            int x_mov = packet[1];
            int y_mov = packet[2];
            
            // sign bits
            if (packet[0] & 0x10) x_mov |= 0xFFFFFF00;  // x
            if (packet[0] & 0x20) y_mov |= 0xFFFFFF00;  // y
            
            // invert y axis
            y_mov = -y_mov;
            
            if (x_mov != 0 || y_mov != 0) {
                // ignore small movements
                if (x_mov >= -2 && x_mov <= 2) x_mov = 0;
                if (y_mov >= -2 && y_mov <= 2) y_mov = 0;
                
                x_mov = x_mov / 4;
                y_mov = y_mov / 4;
                
                if (x_mov != 0 || y_mov != 0) {
                    mouse_state.x += x_mov;
                    mouse_state.y += y_mov;
                }
                
                if (mouse_state.x < 0) mouse_state.x = 0;
                if (mouse_state.y < 0) mouse_state.y = 0;
                if (mouse_state.x >= VGA_WIDTH) mouse_state.x = VGA_WIDTH - 1;
                if (mouse_state.y >= VGA_HEIGHT) mouse_state.y = VGA_HEIGHT - 1;
            }
            
            cycle = 0;
            break;
    }
}

MouseState get_mouse_state(void) {
    return mouse_state;
}

void set_mouse_position(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        mouse_state.x = x;
        mouse_state.y = y;
    }
}
