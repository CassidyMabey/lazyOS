# LazyOS

A lightweight operating system with a graphical user interface, window management, and keyboard/mouse support.

## Features

- Text-mode VGA graphics (80x25)
- Window management system
  - Draggable windows
  - Window stacking (z-order)
  - Window focus management
- Input handling
  - Multiple keyboard layout support (QWERTY, DVORAK, AZERTY)
  - PS/2 mouse support with cursor
  - Configurable settings through GUI
- Memory management
  - Simple heap allocator
  - Dynamic memory allocation
  - Memory block management

## Building

### Prerequisites

- GCC cross-compiler (i686-linux-gnu)
- GRUB2
- QEMU (for testing)
- WSL or Linux environment

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/CassidyMabey/lazyOS.git
cd lazyOS
```

2. Make the build script executable:
```bash
chmod +x build.sh
```

3. Build and run:
```bash
./build.sh
```

This will:
- Compile all source files
- Create a bootable ISO
- Launch QEMU with the ISO

## Usage

### Basic Controls

- **ESC**: Toggle settings sidebar
- **Mouse**: 
  - Click and drag window title bars to move windows
  - Click anywhere in a window to bring it to front
- **Keyboard**:
  - Type in active window
  - ENTER in settings to change keyboard layout

### Window Management

Windows can be:
- Dragged by their title bars
- Brought to front by clicking
- Stacked in any order
- Positioned anywhere on screen

### Keyboard Layouts

Three layouts are supported:
- QWERTY (default)
- DVORAK
- AZERTY

Change layouts through the settings sidebar (ESC to open).


## Future Plans

- [ ] Add color customization
- [ ] Implement file system
- [ ] Add more window controls (minimize, maximize)
- [ ] Support for higher resolution modes
- [ ] Terminal emulator window
- [ ] Simple text editor
- [ ] Basic shell commands
