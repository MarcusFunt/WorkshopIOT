# MR60BHA1 MQTT Bridge (XIAO ESP32-C3)

This sketch turns a Seeed Studio XIAO ESP32-C3 and MR60BHA1 60 GHz radar into an MQTT bridge for Home Assistant. It streams presence, movement, distance, heart rate, and breathing data.

## Hardware
- Board: Seeed Studio XIAO ESP32-C3
- Sensor: Seeed Studio MR60BHA1 (60 GHz mmWave)

### Wiring
| XIAO ESP32-C3 | MR60BHA1 |
| --- | --- |
| 3V3 | 3V3 |
| GND | GND |
| D6  | TX (sensor TX -> board RX) |
| D7  | RX (sensor RX -> board TX) |

### Libraries
Install via Arduino Library Manager:
- **PubSubClient** by Nick O'Leary
- **Seeed-Studio-MR60BHA1-Sensor** (search for `60ghzbreathheart` or add the GitHub repo as a ZIP)

## Uploading
1. Copy `mr60bha1_ha_gateway` into your Arduino sketches folder.
2. Set **Board** to `Seeed XIAO ESP32C3` in the IDE.
3. Fill in Wi-Fi and MQTT details at the top of `mr60bha1_ha_gateway.ino`.
4. Select the correct serial port and upload.

## MQTT topics
Base topic: `homeassistant/xiao/mr60bha1`
- `presence`: `occupied` or `clear`
- `activity`: `moving`, `stationary`, `activity_level_<n>`, or `none`
- `distance_m`: numeric meters (retained)
- `heart_rate_bpm`: numeric bpm (retained)
- `breath_rate_bpm`: numeric bpm (retained)
- `breath_state`: `normal`, `rapid`, `slow`, or `none`
- `state`: JSON snapshot of all fields (retained)

## Home Assistant
A ready-to-use MQTT configuration is provided in [`../../home-assistant/mr60bha1_mqtt.yaml`](../../home-assistant/mr60bha1_mqtt.yaml). Drop it into your Home Assistant config folder and include it from `configuration.yaml`, or copy the entries manually.
