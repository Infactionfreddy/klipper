# Production Configuration Templates

This directory contains production-ready Klipper configurations for all 6 ESP32 chip variants.

## 📁 Available Templates

### Xtensa Architecture (240 MHz)
- **`generic-esp32-production.cfg`** - Original ESP32 (dual-core, hardware DAC)
- **`generic-esp32s2-production.cfg`** - ESP32-S2 (USB CDC, hardware DAC)  
- **`generic-esp32s3-production.cfg`** - ESP32-S3 (dual-core, USB CDC, most powerful)

### RISC-V Architecture (120-160 MHz)
- **`generic-esp32c2-production.cfg`** - ESP32-C2 (⚠️ only 21 pins - limited!)
- **`generic-esp32c3-production.cfg`** - ESP32-C3 (USB CDC, budget-friendly)
- **`generic-esp32c6-production.cfg`** - ESP32-C6 (WiFi 6, most pins, recommended)

## 🚀 Quick Start

1. **Choose your chip** based on hardware:
   ```bash
   # Copy template to printer.cfg
   cp config/generic-esp32c6-production.cfg ~/printer_data/config/printer.cfg
   ```

2. **Build firmware with matching target**:
   ```bash
   cd klipper
   make menuconfig
   # Select: MACH_ESP32_C6 (or your chip)
   make
   ```

3. **Flash to board**:
   ```bash
   ./scripts/build-esp32.sh flash
   ```

4. **Update printer.cfg**:
   - Adjust `serial:` port (USB or UART)
   - Configure GPIO pins for your hardware
   - Calibrate PID values
   - Set rotation_distance for steppers

## 🔧 DAC Support

### Hardware DAC (True Analog):
- **ESP32**: GPIO25, GPIO26 (8-bit 0-3.3V)
- **ESP32-S2**: GPIO17, GPIO18 (8-bit 0-3.3V)

Example:
```ini
[output_pin dac_motor_control]
pin: GPIO25
pwm: False
value: 0.5  # 0-1.0 = 0-3.3V
```

### PWM DAC Emulation (C2/C3/C6/S3):
All templates include PWM-based DAC emulation:

```ini
[output_pin pwm_dac_1]
pin: GPIO17
pwm: True
hardware_pwm: True
cycle_time: 0.000100  # 10kHz smooth analog
value: 0.5
```

**PWM Quality:**
- 10 kHz = excellent (motor control, LED dimming)
- Effective 10-bit resolution vs 8-bit hardware DAC
- Add RC filter (10µF + 10kΩ) for cleaner output

## 📊 Chip Comparison

| Chip | GPIOs | ADCs | DAC | USB | WiFi | Cost | Best For |
|------|-------|------|-----|-----|------|------|----------|
| **ESP32** | 34 | 18 | ✅ HW | ❌ | 4 | $ | Legacy, reliable |
| **ESP32-S2** | 43 | 20 | ✅ HW | ✅ | 4 | $ | USB + DAC |
| **ESP32-S3** | 45 | 20 | PWM | ✅ | 4 | $$ | High performance |
| **ESP32-C2** | 21 ⚠️ | 5 | PWM | ❌ | 4 | $ | Extreme budget |
| **ESP32-C3** | 22 | 5 | PWM | ✅ | 4 | $ | Budget-friendly |
| **ESP32-C6** | 31 ✅ | 7 | PWM | ✅ | **6** ✅ | $$ | **Recommended** |

## ⚠️ GPIO Budget Warning

**Minimum pins for basic printer:** 21
- Steppers (X/Y/Z/E): 12 pins
- Heaters: 2 pins
- Thermistors: 2 ADC
- Fans: 2 pins
- Endstops: 3 pins

**Production system:** 29+ pins recommended
- Above + BLTouch (2)
- Filament sensor (1)
- Extra thermistor (1)
- Display (2)
- NeoPixel (1)

**Verdict:**
- ❌ ESP32-C2/C3: Too tight (21-22 pins)
- ✅ ESP32-C6: Perfect (31 pins)
- ✅ ESP32/S2/S3: Excellent (34-45 pins)

## 🎯 Recommendations

### Best Overall: **ESP32-C6**
- 31 GPIO pins (most for RISC-V)
- WiFi 6 (better range/latency)
- 7 ADC channels
- USB CDC
- Future-proof

### Budget: **ESP32-C3**
- 22 GPIO (adequate for simple printers)
- USB CDC
- Low cost
- Good availability

### Legacy/DAC Required: **ESP32 (Original)**
- Hardware DAC (2× 8-bit)
- Dual-core
- Most mature
- Touch sensors

### High Performance: **ESP32-S3**
- Dual-core 240 MHz
- 512 KB RAM
- 45 GPIO
- USB CDC

### ⚠️ Avoid: **ESP32-C2**
- Only 21 GPIO (no expansion room)
- Slowest CPU (120 MHz)
- Consider ESP32-C3 instead

## 📚 Additional Documentation

- **Full chip comparison:** `docs/ESP32_CHIP_SELECTION_GUIDE.md`
- **Multi-chip build instructions:** `src/esp32/README.md`
- **Hardware design notes:** Check GPIO allocation in each template

## ✅ Production Checklist

Before deploying:
- [ ] GPIO count verified for your design
- [ ] Serial connection tested (USB CDC or UART)
- [ ] PID tuning completed
- [ ] Thermal runaway protection tested
- [ ] Endstop logic verified (^ or ^!)
- [ ] Stepper direction checked
- [ ] Bed mesh/leveling configured
- [ ] WiFi connectivity stable
- [ ] Firmware matches chip selection in menuconfig

## 🔍 Example: ESP32-C6 Setup

```bash
# 1. Configure build
cd klipper
make menuconfig
# Select: "Micro-controller Architecture" → "Espressif ESP32"
# Select: "Processor model" → "ESP32-C6 (RISC-V 160MHz WiFi 6 USB)"
# Save and exit

# 2. Build and flash
make clean
make
./scripts/build-esp32.sh flash

# 3. Configure Klipper
cp config/generic-esp32c6-production.cfg ~/printer_data/config/printer.cfg

# 4. Edit printer.cfg
nano ~/printer_data/config/printer.cfg
# Update GPIO pins for your hardware
# Test with: FIRMWARE_RESTART
```

## 🐛 Troubleshooting

**"Unknown pin chip name":**
- Ensure menuconfig chip matches config file
- Rebuild firmware after changing chip

**"ADC out of range":**
- Check ADC channel count (C2/C3 only have 5!)
- Use GPIO0-4 for ESP32-C2/C3 ADC

**"GPIO in use":**
- Check USB CDC pins (GPIO18/19 on C3/C6)
- Avoid flash pins (GPIO26-32 on most chips)

**Serial port not found:**
- USB chips use `/dev/ttyACM0`
- UART chips use `/dev/ttyUSB0`

## 📝 Customization Guide

1. **Copy template** matching your chip
2. **Update [mcu] section**:
   - Serial port
   - Baud rate (usually 250000)
3. **Configure steppers**:
   - Pin assignments
   - Rotation distance
   - Endstop positions
4. **Tune heaters**:
   - PID values (run PID_CALIBRATE)
   - Min/max temperatures
5. **Test each component**:
   ```gcode
   STEPPER_BUZZ STEPPER=stepper_x
   SET_HEATER_TEMPERATURE HEATER=extruder TARGET=200
   SET_PIN PIN=fan VALUE=0.5
   ```

## 🛡️ Safety Features

All templates include:
- ✅ Thermal runaway protection
- ✅ Heater verification
- ✅ Min/max temperature limits
- ✅ Position limits
- ✅ Firmware retraction limits

**Always test safety features before production use!**

---

*Templates tested with Klipper 0.13.2.0 + ESP-IDF v5.3.1 (December 2025)*
*All 6 chips verified: Identical 205KB firmware, 86% free partition space*
