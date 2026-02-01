# ESP32 Motor Spinner

Stepper motor control with OLED display.

## Hardware

- **Microcontroller**: ESP32-C3 with built-in OLED (0.96" SSD1306 128x64)
- **Motor Driver**: ULN2003 driver board
- **Motor**: 28BYJ-48 5V DC stepper motor

## Wiring

| ESP32-C3 | ULN2003 |
|----------|---------|
| GPIO7    | IN1     |
| GPIO8    | IN2     |
| GPIO9    | IN3     |
| GPIO10   | IN4     |
| 5V       | 5V/VCC  |
| GND      | GND     |

OLED uses GPIO5 (SDA) and GPIO6 (SCL).

## Arduino CLI Setup

Install arduino-cli:
```bash
brew install arduino-cli
```

Add ESP32 board support:
```bash
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

Install required libraries:
```bash
arduino-cli lib install "Adafruit SSD1306" "Adafruit GFX Library"
```

## Compile and Upload

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c3:CDCOnBoot=cdc stepper_motor
arduino-cli upload --fqbn esp32:esp32:esp32c3:CDCOnBoot=cdc --port /dev/cu.usbmodem1101 stepper_motor
```

Note: The port may vary. Find it with:
```bash
ls /dev/cu.usb*
```

## Configuration Notes

- **CDCOnBoot=cdc** is required for ESP32-C3 boards with native USB (no separate USB-serial chip)
- If upload fails with "No serial data received", unplug and replug the USB cable, then try again immediately
- Max reliable speed for 28BYJ-48 is ~1ms per step (~15 RPM). At 0.5ms it stalls.
- **Display offset**: This OLED module has an offset - visible area starts around Y=28. Text at Y=0 is not visible.
