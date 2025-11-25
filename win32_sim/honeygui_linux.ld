/* Linker script for Linux/WSL build */
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
    .shellCommand : {
        _shell_command_start = .;
        KEEP(*(.shellCommand))
        _shell_command_end = .;
    }
}

INSERT AFTER .rodata;
