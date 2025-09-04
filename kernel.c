#include <stdint.h>
#include <system.h>
#include "multiboot.h"
#include "src/drivers/vga.h"
#include "src/drivers/keyboard.h"
#include "src/drivers/mouse.h"
#include "src/drivers/timer.h"
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
    
    // mouse
    init_mouse();
    
    // timer for 60 FPS (1000 Hz / 16.67 ≈ 60 FPS)
    init_timer(1000);
}

void kernel_main(void) {
    // init
    init_memory();
    init_hardware();
    init_gui();
    
    // Create default windows with sizes relative to screen dimensions
    create_window(VGA_WIDTH/4, VGA_HEIGHT/8, VGA_WIDTH/2, VGA_HEIGHT/3, "Welcome to LazyOS");
    create_window(VGA_WIDTH/8, VGA_HEIGHT/4, VGA_WIDTH/3, VGA_HEIGHT/3, "Settings");
    create_window(VGA_WIDTH/6, VGA_HEIGHT/6, VGA_WIDTH/2, VGA_HEIGHT/2, "File Manager");
    
    //draw initial
    draw_desktop();
    redraw_windows();
    
    // 60 FPS timing control
    uint32_t last_frame_time = 0;
    const uint32_t frame_interval = 16; // ~60 FPS (16.67ms)
    
    // Main event loop
    while (1) {
        uint32_t current_time = get_timer_ticks();
        
        // keyboard input (always responsive)
        char key = get_key();
        if (key) {
            handle_input(key);
        }
        
        // handle mouse input (always responsive for smooth movement)
        handle_mouse_packet();
        handle_mouse_input();
        
        // Frame rate control - only update at 60 FPS
        if (current_time - last_frame_time >= frame_interval) {
            last_frame_time = current_time;
            // Any periodic updates can go here
        }
        
        // Small CPU break
        asm("hlt");
    }
}
