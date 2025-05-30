NASM          ?= nasm
LD            ?= ld
GCC           ?= gcc
OBJDUMP       ?= objdump
GDB           ?= gdb
GRUB_MKRESCUE ?= grub-mkrescue
QEMU          ?= qemu-system-x86_64
BEAR          ?= bear
MAKE          ?= make

CFLAGS        := -ffreestanding \
                 -fno-builtin \
                 -fno-omit-frame-pointer \
                 -fno-PIC \
                 -ffunction-sections \
                 -fdata-sections \
                 -fcf-protection=none \
                 -fno-stack-protector \
                 \
                 -nostdlib \
                 -nostdinc \
                 \
                 -std=c99 \
                 -Os \
                 \
                 -m64 \
                 -mcmodel=kernel \
                 -mno-red-zone \
                 -mno-sse \
                 -mno-mmx \
                 -mno-sse2 \
                 -mno-avx \
                 \
                 -Wall \
                 -Wextra \
                 -Wno-unused-function \
                 -Wno-unused-variable \
                 -Wpointer-arith \
                 -Wcast-align \
                 -Wredundant-decls \
                 -Wundef \
                 -Wconversion \
                 -Wmissing-prototypes \
                 -Wmissing-declarations \
                 -Wstrict-prototypes \
                 -Wuninitialized \
                 -Wreturn-type \
                 -Wwrite-strings \
                 -Wstrict-overflow=1 \
                 -Wshift-overflow \
                 -Wshift-negative-value \
                 -Wstrict-aliasing=2 \
                 -Wshadow \
                 -Wformat=2 \
                 -Werror \
                 -g
LDFLAGS       := -z noexecstack --gc-sections
ASMFLAGS      := -g
QEMUFLAGS     := -serial stdio -m 1024 -d int -no-reboot
GDBFLAGS      := -ex "file dist/x86_64/kernel.bin" -ex "target remote localhost:1234"

CFLAGS_FILE := build/.cflags
ASMFLAGS_FILE := build/.asmflags
LDFLAGS_FILE := build/.ldflags

kernel_source_files     := $(shell find src/impl/kernel -name *.c)
kernel_object_files     := $(patsubst src/impl/kernel/%.c, build/kernel/%.c.o, $(kernel_source_files))

x86_64_c_source_files   := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_object_files   := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.c.o, $(x86_64_c_source_files))

x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.asm.o, $(x86_64_asm_source_files))

x86_64_object_files     := $(x86_64_c_object_files) $(x86_64_asm_object_files)

$(CFLAGS_FILE): Makefile
	@mkdir -p $(@D)
	@printf '%s\n' '$(CFLAGS)' | cmp -s - $@ || printf '%s\n' '$(CFLAGS)' > $@

$(ASMFLAGS_FILE): Makefile
	@mkdir -p $(@D)
	@printf '%s\n' '$(ASMFLAGS)' | cmp -s - $@ || printf '%s\n' '$(ASMFLAGS)' > $@

$(LDFLAGS_FILE): Makefile
	@mkdir -p $(@D)
	@printf '%s\n' '$(LDFLAGS)' | cmp -s - $@ || printf '%s\n' '$(LDFLAGS)' > $@


$(kernel_object_files): build/kernel/%.c.o : src/impl/kernel/%.c $(CFLAGS_FILE)
	@mkdir -p $(@D)
	$(GCC) $(CFLAGS) -I include -c $< -o $@

$(x86_64_c_object_files): build/x86_64/%.c.o : src/impl/x86_64/%.c $(CFLAGS_FILE)
	@mkdir -p $(@D)
	$(GCC) $(CFLAGS) -I include -c $< -o $@

$(x86_64_asm_object_files): build/x86_64/%.asm.o : src/impl/x86_64/%.asm $(ASMFLAGS_FILE)
	@mkdir -p $(@D)
	$(NASM) -f elf64 $(ASMFLAGS) $< -o $@

KERNEL_BIN := dist/x86_64/kernel.bin
KERNEL_ISO := dist/x86_64/kernel.iso
KERNEL_LST := kernel.lst

$(KERNEL_BIN): $(x86_64_object_files) $(kernel_object_files) targets/x86_64/linker.ld $(LDFLAGS_FILE)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ -T targets/x86_64/linker.ld $(x86_64_object_files) $(kernel_object_files)

$(KERNEL_ISO): $(KERNEL_BIN)
	@mkdir -p targets/x86_64/iso/boot
	cp $(KERNEL_BIN) targets/x86_64/iso/boot/kernel.bin
	$(GRUB_MKRESCUE) /usr/lib/grub/i386-pc -o $@ targets/x86_64/iso

$(KERNEL_LST): $(KERNEL_BIN)
	$(OBJDUMP) -D -S $< > $@

.PHONY: build-x86_64
build-x86_64: $(KERNEL_ISO) $(KERNEL_LST)

.PHONY: run-x86_64
run-x86_64: build-x86_64
	$(QEMU) -cdrom $(KERNEL_ISO) $(QEMUFLAGS)

.PHONY: all
all: build-x86_64

.PHONY: clean
clean:
	rm -rf build/ dist/ targets/*/iso/boot/kernel.bin $(KERNEL_LST)
	rm -f $(CFLAGS_FILE) $(ASMFLAGS_FILE) $(LDFLAGS_FILE)

.PHONY: bear
bear: clean
	$(BEAR) -- $(MAKE) $(CFLAGS_FILE) $(ASMFLAGS_FILE) $(LDFLAGS_FILE) build-x86_64