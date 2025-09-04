#include "src/drivers/keyboard.h"
#include "src/drivers/vga.h"
#include "src/gui/gui.h"

void kernel_main(void) {
    // Initialize GUI system
    init_gui();
    
    // Create a welcome window
    create_window(10, 5, 40, 10, "Welcome to LazyOS");
    
    // Main input loop
    while (1) {
        char key = get_key();
        handle_input(key);
    }
}
