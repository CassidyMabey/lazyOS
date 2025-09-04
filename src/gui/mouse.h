#ifndef GUI_MOUSE_H
#define GUI_MOUSE_H

#include <stdint.h>
#include "../drivers/mouse.h"
#include "gui.h"

void handle_mouse_input(void);
void check_window_interaction(int mouse_x, int mouse_y, uint8_t buttons);

#endif
