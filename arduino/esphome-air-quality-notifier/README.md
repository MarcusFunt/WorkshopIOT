# Air Quality Notifier (ESPHome)

This ESPHome project monitors air quality with a SEN54, shows the readings on a 16x2 HD44780 LCD (PCF8574 backpack), and uses 20 WS2812 NeoPixels as a visual notification bar.

## Hardware
- ESP32 DevKit (change the `esp32.board` if you use a different module)
- Sensirion SEN54 (I2C)
- 20x WS2812/NeoPixel LEDs on a single data line
- 16x2 HD44780 LCD with PCF8574 I2C backpack (default address `0x27`)

## Wiring
| Signal            | ESP32 Pin | Notes                                  |
| ----------------- | --------- | -------------------------------------- |
| NeoPixel Data     | GPIO5     | 5V-tolerant LEDs, add level shifter if needed |
| NeoPixel Power    | 5V & GND  | Provide adequate current for 20 LEDs   |
| I2C SDA           | GPIO21    | Shared by SEN54 and LCD backpack       |
| I2C SCL           | GPIO22    | Shared by SEN54 and LCD backpack       |
| LCD Backlight     | GPIO16    | Drives a simple GPIO-controlled backlight |

Adjust pins in `air_quality_notifier.yaml` as needed.

## Behavior
- The LCD updates with PM2.5 on the first line and temperature/humidity on the second.
- NeoPixels show air quality by color (green <12 µg/m³, amber <35, orange <55, red otherwise). Colors update whenever PM2.5 changes and every 10 seconds.
- A backlight switch entity (`LCD Backlight`) is exposed via ESPHome API to toggle the LCD backlight.

## Usage
1. Install ESPHome and set your Wi-Fi credentials in `air_quality_notifier.yaml`.
2. Flash the ESP32: `esphome run arduino/esphome-air-quality-notifier/air_quality_notifier.yaml`.
3. Pair with Home Assistant via ESPHome API or use the fallback AP if Wi-Fi is unavailable.
