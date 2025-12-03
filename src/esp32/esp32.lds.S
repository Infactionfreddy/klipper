// ESP32 linker script
// Copyright (C) 2024 Frederick <frederick@example.com>
// This file may be distributed under the terms of the GNU GPLv3 license.

OUTPUT_FORMAT("elf32-xtensa-le")
OUTPUT_ARCH(xtensa)

MEMORY
{
    /* ESP32 memory layout */
    iram (RX)  : ORIGIN = 0x40080000, LENGTH = 128K
    dram (RW)  : ORIGIN = 0x3FFB0000, LENGTH = 176K
    flash (RX) : ORIGIN = 0x400D0000, LENGTH = 4M
}

SECTIONS
{
    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.entry.text)
        *(.init .init.*)
        *(.text .text.*)
        *(.iram1 .iram1.*)
        *(.gnu.linkonce.t.*)
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
        _etext = .;
    } > iram AT > flash

    .data : AT(LOADADDR(.text) + SIZEOF(.text)) {
        . = ALIGN(4);
        _sdata = .;
        _data_start = .;
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        *(.dram1 .dram1.*)
        . = ALIGN(4);
        _edata = .;
        _data_end = .;
    } > dram

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _sbss = .;
        _bss_start = .;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        _bss_end = .;
    } > dram

    _heap_start = _bss_end;
    _heap_end = ORIGIN(dram) + LENGTH(dram);
}

    /* Stack */
    .stack (NOLOAD) : {
        . = ALIGN(8);
        _stack_start = .;
        . = . + CONFIG_STACK_SIZE;
        . = ALIGN(8);
        _stack_end = .;
    } > dram

    /DISCARD/ : {
        *(.ARM.exidx .ARM.exidx.* .ARM.extab.*)
    }
}

_estack = _stack_end;
