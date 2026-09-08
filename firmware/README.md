# nRF24 Protocol Hacker (MouseJack) Firmware

## Purpose

Sniff and analyze nRF24L01+ wireless HID traffic, classify mouse/keyboard reports, and log device activity (analysis only in demo).

## Board

- **Board**: ESP32 NodeMCU + nRF24L01+
- **FQBN**: `esp32:esp32:esp32`
- **Sketch**: `h4_mousejack/h4_mousejack.ino`

## Wiring

```
nRF24L01+ (hardware VSPI): CLK->D18, MOSI->D23, MISO->D19, CSN->D26, CE->D27, VCC->3V3, GND->GND
```

## Build

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/h4_mousejack
# upload (example, ESP32-C6):
# arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyACM0 firmware/h4_mousejack
```

## Runtime

See the root README "IMPORTANT" section before powering on. This firmware is
for authorized own-lab study. Serial console exposes the interactive command
set described in the root README. All identifiers in the sketch are
placeholders (`lab-*` SSIDs, `00:11:22:33:44:55`, RFC 5737 / example.com).
