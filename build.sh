
set -e

# Define build artifacts and compiler flags
KERNEL_BIN="kernel.bin"
ISO_DIR="isodir"
ISO_FILE="lazyOS.iso"
GRUB_CFG="$ISO_DIR/boot/grub/grub.cfg"

# Ensure include directory exists
mkdir -p include

# Compiler and linker flags
CFLAGS="-ffreestanding -m32 -Wall -Wextra -I. -nostdinc -fno-builtin -I./include -fno-common"
LDFLAGS="-m elf_i386 -T linker.ld -nostdlib -e kernel_main"

# Verify header files exist
for header in string.h stddef.h stdint.h; do
    if [ ! -f "include/$header" ]; then
        echo "Error: Missing required header: $header"
        exit 1
    fi
done

# Ensure we have all necessary headers
for header in stddef.h stdint.h; do
    if [ ! -f "include/$header" ]; then
        echo "Error: Missing required header: $header"
        exit 1
    fi
done

# Find all .c files recursively
echo "Finding source files..."
SOURCES=()

# Add root kernel.c first if it exists
if [ -f "kernel.c" ]; then
    SOURCES+=("kernel.c")
fi

# Find all source files in src directory
while IFS= read -r -d '' file; do
    # Skip src/kernel.c if it exists
    if [ "$(basename "$file")" != "kernel.c" ]; then
        SOURCES+=("$file")
    fi
done < <(find src -type f -name "*.c" -print0 | sort -z)

# Display found source files
echo "Found source files:"
for src in "${SOURCES[@]}"; do
    echo "  $src"
done

# Generate object file names
OBJECTS=()
for src in "${SOURCES[@]}"; do
    obj="${src%.c}.o"
    OBJECTS+=("$obj")
done

# Function to clean up build artifacts
cleanup() {
    echo "Cleaning up build files..."
    # Clean all object files
    for obj in "${OBJECTS[@]}"; do
        rm -f "$obj"
    done
    rm -f $KERNEL_BIN
    rm -rf $ISO_DIR $ISO_FILE
}

# Clean up old files before building
cleanup

# Trap to ensure cleanup on script exit (normal or error)
trap cleanup EXIT

# Compile all source files
echo "Compiling source files..."
for src in "${SOURCES[@]}"; do
    obj="${src%.c}.o"
    echo "  Compiling $src..."
    i686-linux-gnu-gcc $CFLAGS -c "$src" -o "$obj"
done

# Link all object files
echo "Linking kernel..."
i686-linux-gnu-ld $LDFLAGS -o $KERNEL_BIN "${OBJECTS[@]}"

echo "Setting up ISO folder..."
mkdir -p $ISO_DIR/boot/grub
cp $KERNEL_BIN $ISO_DIR/boot/

cat > $GRUB_CFG << EOF
menuentry "lazyOS" {
    multiboot /boot/$KERNEL_BIN
}
EOF

echo "Creating bootable ISO..."
grub-mkrescue -o $ISO_FILE $ISO_DIR

echo "Running in QEMU..."
qemu-system-i386 -cdrom $ISO_FILE

# Cleanup will happen automatically when script exits due to the trap
