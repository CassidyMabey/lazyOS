
set -e

KERNEL_BIN="kernel.bin"
ISO_DIR="isodir"
ISO_FILE="lazyOS.iso"
GRUB_CFG="$ISO_DIR/boot/grub/grub.cfg"


mkdir -p include


CFLAGS="-ffreestanding -m32 -Wall -Wextra -I. -nostdinc -fno-builtin -I./include -fno-common"
LDFLAGS="-m elf_i386 -T linker.ld -nostdlib -e kernel_main"

for header in string.h stddef.h stdint.h; do
    if [ ! -f "include/$header" ]; then
        echo "Error: Missing required header: $header"
        exit 1
    fi
done

for header in stddef.h stdint.h; do
    if [ ! -f "include/$header" ]; then
        echo "Error: Missing required header: $header"
        exit 1
    fi
done

echo "Finding source files..."
SOURCES=()

# add kernel first
if [ -f "kernel.c" ]; then
    SOURCES+=("kernel.c")
fi

# find all source files
while IFS= read -r -d '' file; do
    if [ "$(basename "$file")" != "kernel.c" ]; then
        SOURCES+=("$file")
    fi
done < <(find src -type f -name "*.c" -print0 | sort -z)

echo "Found source files:"
for src in "${SOURCES[@]}"; do
    echo "  $src"
done

OBJECTS=()
for src in "${SOURCES[@]}"; do
    obj="${src%.c}.o"
    OBJECTS+=("$obj")
done

cleanup() {
    echo "Cleaning up build files..."
    for obj in "${OBJECTS[@]}"; do
        rm -f "$obj"
    done
    rm -f $KERNEL_BIN
    rm -rf $ISO_DIR $ISO_FILE
}

cleanup

trap cleanup EXIT

echo "Compiling source files..."
for src in "${SOURCES[@]}"; do
    obj="${src%.c}.o"
    echo "  Compiling $src..."
    i686-linux-gnu-gcc $CFLAGS -c "$src" -o "$obj"
done

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