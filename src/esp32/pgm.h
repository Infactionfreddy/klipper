#ifndef __ESP32_BOARD_PGM_H
#define __ESP32_BOARD_PGM_H

// ESP32 hat keinen PROGMEM-Speicher wie AVR
// Alle Konstanten sind normal im Flash/RAM
#define READP(VAL) (VAL)
#define PROGMEM
#define PSTR(S) (S)

#endif // __ESP32_BOARD_PGM_H
