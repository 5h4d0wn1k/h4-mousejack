# H4 — nRF24 Protocol Hacker (MouseJack)

Sniff and inject wireless keyboard/mouse traffic using nRF24L01+.

## Overview

This project implements a MouseJack-style attack tool that:
- Scans all 41 nRF24 channels for wireless peripheral traffic
- Captures and analyzes HID reports from keyboards/mice
- Injects keystrokes and mouse movements
- Tracks discovered devices

**WARNING: Educational use only. Test on your own devices.**

## Hardware

| Component | Connection | Role |
|-----------|------------|------|
| ESP32 NodeMCU | Main board | Control + analysis |
| nRF24L01+ | Hardware VSPI | 2.4 GHz radio |

## nRF24 Wiring

```
nRF24 Pin → NodeMCU Pin
────────────────────────
CLK      → D18 (GPIO18)
MOSI     → D23 (GPIO23)
MISO     → D19 (GPIO19)
CSN      → D26 (GPIO26)
CE       → D27 (GPIO27)
VCC      → 3.3V
GND      → GND
```

## Serial Commands

```
scan          - Scan all 41 nRF24 channels
sniff         - Start channel hopping sniff
sniff CH      - Sniff specific channel
stop          - Stop sniffing
devices       - List discovered devices
select N      - Select device N
inject key XX - Inject key press (hex code)
inject move X Y - Inject mouse movement
inject click B - Inject mouse click (1=L, 2=R, 3=M)
clear         - Clear all captures
help          - Show commands
```

## Common Channels

| Channel | Frequency | Typical Device |
|---------|-----------|----------------|
| 8 | 2408 MHz | Logitech |
| 10 | 2410 MHz | Dell |
| 19 | 2419 MHz | Microsoft |
| 25 | 2425 MHz | Apple (partial) |
| 37 | 2437 MHz | Various |

## HID Report Formats

### Mouse (3 bytes)
```
[Buttons] [X-axis] [Y-axis]
  0x01 = Left
  0x02 = Right
  0x04 = Middle
```

### Keyboard (8 bytes)
```
[Modifier] [Reserved] [Key1] [Key2] [Key3] [Key4] [Key5] [Key6]
  0x01 = Ctrl
  0x02 = Shift
  0x04 = Alt
  0x08 = GUI
```

## Example Session

```
=== H4 — nRF24 Protocol Hacker ===
nRF24 initialized successfully

[CH: 8] Traffic: 00 00 04 00 00 00 00 00
[NEW DEVICE] CH:8 Type: Keyboard

[PKT #1] LEN:3 HEX: 01 05 00
  MOUSE: Buttons=01 X=5 Y=0 [LEFT]

[INJECT] Key press: 0x04 (A key)
  Sent!
```

## Build & Flash

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 h4_mousejack
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 h4_mousejack
```

## Research Value

- **H4 — MouseJack**: This project
- **P2 — ESP-NOW Security SoK**: Analyze wireless protocol security
- **H14 — RF Lockpick**: Extend to other 2.4 GHz protocols

## References

- MouseJack (Bastille Networks)
- nRF24L01+ Datasheet
- USB HID Specification

## License

MIT
