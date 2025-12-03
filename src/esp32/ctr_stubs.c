// Compile Time Request (CTR) Stubs für ESP32
//
// Dies sind temporäre Stubs für die CTR-Funktionen.
// In einem vollständigen Build werden diese von
// scripts/extract_compile_time_requests.py generiert.
//
// Copyright (C) 2024  Your Name <your.email@example.com>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include <stdint.h>
#include <string.h>
#include "autoconf.h"
#include "compiler.h"

// Dummy encoder für CTR
static uint8_t dummy_encoder[] = {0x00};

// Initialisierungsfunktionen aufrufen
void
ctr_run_initfuncs(void)
{
    // In einem vollständigen Build würde hier code stehen
    // der alle DECL_INIT Funktionen aufruft
}

// Task-Funktionen aufrufen
void
ctr_run_taskfuncs(void)
{
    // In einem vollständigen Build würde hier code stehen
    // der alle DECL_TASK Funktionen aufruft
}

// Shutdown-Funktionen aufrufen
void
ctr_run_shutdownfuncs(void)
{
    // In einem vollständigen Build würde hier code stehen
    // der alle DECL_SHUTDOWN Funktionen aufruft
}

// Encoder Lookup
const void *
ctr_lookup_encoder(const char *name)
{
    // Dummy encoder zurückgeben
    return dummy_encoder;
}

// Static String Lookup
const char *
ctr_lookup_static_string(const char *name)
{
    // Einfach den Namen zurückgeben
    return name;
}
