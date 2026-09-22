> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**
> This project exists for education, research, and **defense of systems you own
> or hold explicit written authorization to assess**. Unauthorized use is
> prohibited and may be illegal. Read [ETHICS.md](ETHICS.md) and
> [SCOPE.md](SCOPE.md) before use. Use at your own risk; **AS IS**, no warranty.

# H4 — nRF24 Protocol Hacker (Mousejack Research Lab)

[![License](https://img.shields.io/github/license/5h4d0wn1k/h4-mousejack)](LICENSE)
[![Stars](https://img.shields.io/github/stars/5h4d0wn1k/h4-mousejack)](https://github.com/5h4d0wn1k/h4-mousejack/stargazers)
[![Last Commit](https://img.shields.io/github/last-commit/5h4d0wn1k/h4-mousejack)](https://github.com/5h4d0wn1k/h4-mousejack/commits/master)
[![Issues](https://img.shields.io/github/issues/5h4d0wn1k/h4-mousejack)](https://github.com/5h4d0wn1k/h4-mousejack/issues)

**H4** is a wireless-security research lab for 2.4 GHz peripheral injection
(Mousejack-class attacks): an ESP32 + nRF24L01+ firmware that scans all 41
nRF24 channels, sniffs HID reports, and injects keystrokes — paired with an
offline Python host helper for HID payload classification. Authorized
own-device testing only.

## Why H4?

Wireless keyboards and mice are a common, often unencrypted attack surface —
and a great teaching case for RF security. H4 reproduces the full MouseJack
research flow in a controlled lab: the ESP32 firmware hops all 41 nRF24
channels, captures and displays HID reports (mouse buttons/deltas and keyboard
modifier+key codes), and supports injection experiments against devices you
own. The companion `host/h4_cli.py` classifies captured HID payloads offline,
keeping the analysis reproducible, scriptable, and radio-free.

## Features

- **41-channel nRF24 scanning** — hops all nRF24L01+ channels used by wireless
  peripherals (`firmware/h4_mousejack/h4_mousejack.ino`).
- **HID report capture** — decodes 3-byte mouse and 8-byte keyboard HID
  reports (buttons, deltas, modifiers, key codes).
- **Injection engine** — transmits keyboard/mouse HID payloads in lab injection
  experiments.
- **Device tracking** — records discovered peripherals and capture channels.
- **Serial interface** — commands: `help`, `scan`, `sniff`, `stop`.
- **Offline HID classifier** — `host/h4_cli.py` classifies payload files as
  MOUSE/KEYBOARD/UNKNOWN (`--demo`, `--file`).
- **Unit tests** — `python3 -m unittest discover -s tests`.

## Hardware & Wiring

| Component | Connection | Role |
|-----------|------------|------|
| ESP32 NodeMCU | Main board | Control + analysis |
| nRF24L01+ | Hardware VSPI | 2.4 GHz radio |

```
nRF24 Pin   →  NodeMCU Pin
─────────────────────────────
CLK          →  D18 (GPIO18)
MOSI         →  D23 (GPIO23)
MISO         →  D19 (GPIO19)
CSN          →  D26 (GPIO26)
CE           →  D27 (GPIO27)
VCC          →  3.3V
GND          →  GND
```

## Quickstart

### Prerequisites

- ESP32 NodeMCU + nRF24L01+ board, Arduino CLI with `esp32:esp32:esp32` core
- Python 3.8+ for the host helper

### Flash the firmware

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/h4_mousejack
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 firmware/h4_mousejack
```

### Classify captured HID payloads (offline)

```bash
python3 host/h4_cli.py --demo
python3 host/h4_cli.py --file fixtures/nrf24_capture.log
```

### Tests

```bash
python3 -m unittest discover -s tests
```

## Project Structure

- `firmware/h4_mousejack/h4_mousejack.ino` — ESP32 nRF24 scanner/sniffer/injector.
- `host/h4_cli.py`, `host/hw_common.py` — offline HID payload classification CLI.
- `fixtures/nrf24_capture.log` — sample own-lab capture for analysis.
- `docs/ARCHITECTURE.md` — design notes.
- `tests/` — host helper unit tests.

## Documentation

- [Architecture](docs/ARCHITECTURE.md)
- [Firmware notes](firmware/README.md)
- [ETHICS.md](ETHICS.md), [SCOPE.md](SCOPE.md), [SECURITY.md](SECURITY.md)

## Contributing

Contributions for educational and authorized RF-security research are welcome.
See [CONTRIBUTING.md](CONTRIBUTING.md) and
[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## License

MIT License — see [LICENSE](LICENSE) for details.

> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**