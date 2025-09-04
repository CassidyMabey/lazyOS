
set -e


KERNEL_SRC="kernel.c"
KERNEL_OBJ="kernel.o"
KERNEL_BIN="kernel.bin"
ISO_DIR="isodir"
ISO_FILE="lazyOS.iso"
GRUB_CFG="$ISO_DIR/boot/grub/grub.cfg"


echo "Cleaning old build files..."
rm -rf $KERNEL_OBJ $KERNEL_BIN $ISO_DIR $ISO_FILE


echo "Compiling kernel..."
i686-linux-gnu-gcc -ffreestanding -c $KERNEL_SRC -o $KERNEL_OBJ -m32 -Wall -Wextra


echo "Linking kernel..."
i686-linux-gnu-ld -m elf_i386 -T linker.ld -o $KERNEL_BIN -nostdlib $KERNEL_OBJ


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
