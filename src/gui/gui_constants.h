#ifndef GUI_CONSTANTS_H
#define GUI_CONSTANTS_H

// box drawing characters
#define BOX_HLINE     0xC4    // ─
#define BOX_VLINE     0xB3    // │
#define BOX_TLCORNER  0xDA    // ┌
#define BOX_TRCORNER  0xBF    // ┐
#define BOX_BLCORNER  0xC0    // └
#define BOX_BRCORNER  0xD9    // ┘
#define BOX_SHADE     0xB1    // ░

// random characters
#define BOX_DHLINE    0xCD    // ═
#define BOX_DVLINE    0xBA    // ║
#define BOX_DTLCORNER 0xC9    // ╔
#define BOX_DTRCORNER 0xBB    // ╗
#define BOX_DBLCORNER 0xC8    // ╚
#define BOX_DBRCORNER 0xBC    // ╝

// special characters
#define CHAR_ARROW_RIGHT 0x10  // ►
#define CHAR_BULLET      0x07  // •
#define CHAR_BLOCK      0xDB   // █

// taskbar icons
#define ICON_FILES    0x0F  // file icon
#define ICON_CMD      0x0E  // cmd icon
#define ICON_SETTINGS 0x0D  // settings icon

// taskbar constants
#define TASKBAR_HEIGHT 1
#define ICON_WIDTH 10
#define TASKBAR_Y (VGA_HEIGHT - TASKBAR_HEIGHT)

#endif
