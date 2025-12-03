OUTPUT_FORMAT("elf32-xtensa-le")
OUTPUT_ARCH(xtensa)

MEMORY
{
    iram (RX)  : ORIGIN = 0x40080000, LENGTH = 128K
    dram (RW)  : ORIGIN = 0x3FFB0000, LENGTH = 176K
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
    } > iram

    .data : {
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
