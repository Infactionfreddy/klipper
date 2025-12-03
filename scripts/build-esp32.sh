#!/bin/bash
# Klipper ESP32 Build Script
#
# ESP32 benötigt ESP-IDF statt dem Standard-Makefile-Build
# Dieses Script prüft die ESP-IDF Installation und baut die Firmware

set -e

# Farben für Output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Parse Argumente
CLEAN=false
FLASH=false
MONITOR=false
PORT=""
VERBOSE=false
TARGET=""

for arg in "$@"; do
    case $arg in
        clean)
            CLEAN=true
            ;;
        flash)
            FLASH=true
            ;;
        monitor)
            MONITOR=true
            ;;
        -p=*|--port=*)
            PORT="${arg#*=}"
            ;;
        -t=*|--target=*)
            TARGET="${arg#*=}"
            ;;
        -v|--verbose)
            VERBOSE=true
            ;;
        -h|--help)
            echo "Usage: $0 [clean] [flash] [monitor] [-t=TARGET] [-p=/dev/ttyUSB0] [-v]"
            echo ""
            echo "Options:"
            echo "  clean       Clean build directory vor dem Build"
            echo "  flash       Flash die Firmware nach dem Build"
            echo "  monitor     Öffne Serial Monitor nach dem Flash"
            echo "  -t, --target Target chip (esp32, esp32s2, esp32s3, esp32c2, esp32c3, esp32c6)"
            echo "  -p, --port  Serial Port (z.B. -p=/dev/ttyUSB0)"
            echo "  -v          Verbose output (zeige alle Build-Logs)"
            echo "  -h, --help  Zeige diese Hilfe"
            echo ""
            echo "Beispiele:"
            echo "  $0                    # Nur Build"
            echo "  $0 clean              # Clean + Build"
            echo "  $0 -t=esp32c3         # Build für ESP32-C3"
            echo "  $0 flash              # Build + Flash"
            echo "  $0 flash monitor      # Build + Flash + Monitor"
            echo "  $0 -t=esp32s3 -p=/dev/ttyUSB0 flash monitor"
            exit 0
            ;;
    esac
done

# Banner
echo -e "${BLUE}"
echo "=================================================="
echo "  Klipper ESP32 Build Script"
echo "=================================================="
echo -e "${NC}"

# Prüfe ob ESP-IDF installiert ist
if [ -z "$IDF_PATH" ]; then
    echo -e "${RED}ERROR: ESP-IDF ist nicht geladen!${NC}"
    echo ""
    echo "ESP32 benötigt ESP-IDF zum Bauen."
    echo "Bitte installiere ESP-IDF:"
    echo ""
    echo "  1. https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/"
    echo "  2. Lade ESP-IDF environment:"
    echo "     ${BLUE}. \$HOME/esp/esp-idf/export.sh${NC}"
    echo ""
    exit 1
fi

echo -e "${GREEN}✓${NC} ESP-IDF: $IDF_PATH"

# Prüfe ESP-IDF Version
IDF_VERSION=$(cd $IDF_PATH && git describe --tags 2>/dev/null || echo "unknown")
echo -e "${GREEN}✓${NC} Version: $IDF_VERSION"

# Prüfe Toolchain
if command -v xtensa-esp32-elf-gcc &> /dev/null; then
    GCC_VERSION=$(xtensa-esp32-elf-gcc --version | head -n1 | awk '{print $2}')
    echo -e "${GREEN}✓${NC} Toolchain (Xtensa): xtensa-esp32-elf-gcc $GCC_VERSION"
fi
if command -v riscv32-esp-elf-gcc &> /dev/null; then
    GCC_VERSION=$(riscv32-esp-elf-gcc --version | head -n1 | awk '{print $2}')
    echo -e "${GREEN}✓${NC} Toolchain (RISC-V): riscv32-esp-elf-gcc $GCC_VERSION"
fi

echo ""

# Set Target falls angegeben
if [ -n "$TARGET" ]; then
    echo -e "${BLUE}Setting target to $TARGET...${NC}"
    idf.py set-target $TARGET
    echo -e "${GREEN}✓${NC} Target set to $TARGET"
    echo ""
fi

# Wechsle ins Projektverzeichnis
cd "$(dirname "$0")/.."

# Prüfe und entferne altes main/ Verzeichnis (ESP-IDF Stub, wird nicht mehr benötigt)
if [ -d "main" ]; then
    echo -e "${YELLOW}⚠${NC}  Altes main/ Verzeichnis gefunden"
    rm -rf main.old
    mv main main.old
    echo -e "${GREEN}✓${NC} main/ verschoben nach main.old"
    echo ""
fi

# Clean Build
if [ "$CLEAN" = true ]; then
    echo -e "${BLUE}Cleaning build directory...${NC}"
    rm -rf build sdkconfig
    echo -e "${GREEN}✓${NC} Build directory cleaned"
    echo ""
fi

# Build
echo -e "${BLUE}Building Klipper for ESP32...${NC}"
echo ""

if [ "$VERBOSE" = true ]; then
    idf.py build
else
    # Zeige nur wichtige Build-Schritte
    idf.py build 2>&1 | grep -E "Executing|Building|Linking|Generating.*\.bin|Project build complete|bytes.*free|error:" || true
fi

# Prüfe ob Build erfolgreich war
if [ ! -f "build/klipper-esp32.bin" ]; then
    echo ""
    echo -e "${RED}=================================================="
    echo "  Build fehlgeschlagen!"
    echo "==================================================${NC}"
    echo ""
    echo "Führe für Details aus: $0 -v"
    exit 1
fi

# Build Summary
echo ""
echo -e "${GREEN}=================================================="
echo "  Build erfolgreich!"
echo "==================================================${NC}"
echo ""
FILESIZE=$(ls -lh build/klipper-esp32.bin | awk '{print $5}')
echo -e "Firmware: ${GREEN}build/klipper-esp32.bin${NC} ($FILESIZE)"
echo ""

# Flash falls angefordert
if [ "$FLASH" = true ]; then
    echo -e "${BLUE}Flashing ESP32...${NC}"
    if [ -n "$PORT" ]; then
        idf.py -p $PORT flash
    else
        idf.py flash
    fi
    echo -e "${GREEN}✓${NC} Flash erfolgreich"
    echo ""
fi

# Monitor falls angefordert
if [ "$MONITOR" = true ]; then
    echo -e "${BLUE}Starting Monitor (Ctrl+] zum Beenden)${NC}"
    echo ""
    if [ -n "$PORT" ]; then
        idf.py -p $PORT monitor
    else
        idf.py monitor
    fi
else
    # Zeige Flash-Befehle nur wenn nicht geflasht wurde
    if [ "$FLASH" = false ]; then
        echo "Flash-Befehle:"
        echo "  ${GREEN}./scripts/build-esp32.sh flash${NC}"
        echo "  ${GREEN}idf.py flash${NC}"
        echo "  ${GREEN}idf.py -p /dev/ttyUSB0 flash${NC}"
        echo ""
        echo "Monitor: ${GREEN}idf.py monitor${NC}"
        echo ""
    fi
fi
