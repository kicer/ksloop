/* Label for the program's entry point */
ENTRY(reset_handler)

/* End of RAM/Start of stack */
/* (4KB SRAM) */
_estack = 0x20002000;

/* Set minimum size for stack and dynamic memory. */
/* (The linker will generate an error if there is
 * less than this much RAM leftover.) */
/* (2KB) */
_Min_Leftover_RAM = 0x800;

MEMORY
{
    FLASH ( rx )      : ORIGIN = 0x00000000, LENGTH = 64K
    RAM ( rxw )       : ORIGIN = 0x20000000, LENGTH = 8K
}

INCLUDE "sections.ld"
