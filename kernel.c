#include <stdint.h>
#include <system.h>
#include "multiboot.h"
#include "src/drivers/vga.h"
#include "src/drivers/keyboard.h"
#include "src/drivers/mouse.h"
#include "src/gui/gui.h"
#include "src/memory/memory.h"

static void init_hardware(void);
__attribute__((section(".multiboot")))
const multiboot_header mb_header = {
    .magic = MULTIBOOT_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
};

static void init_hardware(void) {
    //init vga
    init_vga();
    
    // keyboard
    init_keyboard();
}

void kernel_main(void) {
    // init
    init_memory();
    init_hardware();
    init_gui();
    
    // create default windows
    create_window(10, 5, 40, 10, "Welcome to LazyOS");
    create_window(15, 8, 35, 8, "Settings");
    create_window(5, 3, 30, 12, "File Manager");
    
    //draw initial
    draw_desktop();
    redraw_windows();
    
    while (1) {
        // keyboard input
        char key = get_key();
        if (key) {
            handle_input(key);
        }
        
        // handle mouse input
        handle_mouse_packet();
        handle_mouse_input();
    }

    while (1) {
        
        if (inb(KBD_STATUS_PORT) & 1) {
            uint8_t scancode = inb(KBD_DATA_PORT);
            
            
            if (!(scancode & 0x80)) {
                if (scancode == KEY_ENTER) {
                    putchar('\n');
                } else if (scancode == KEY_BACKSPACE) {
                    clear_screen();  
                } else {
                    char c = scancode_to_ascii(scancode);
                    if (c != 0) {
                        putchar(c);
                    }
                }
            }
        }
    }
}
