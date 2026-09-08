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

## Live Lab Test Plan

Run ONLY on an isolated, authorized own-lab bench against devices, networks,
and spectrum **you own**. No third-party callers, bystanders, or spectrum users
may be within range of any test transmission.

1. **Isolate** - Put the DUT in a shielded/Faraday enclosure or a room with no
   third-party devices in range. Use attenuators on any transmit path.
2. **Own devices only** - Every target (AP, remote, tag, GPS module, drone FC,
   receiver) must be your own hardware.
3. **Lowest power, shortest duration** - Start at minimum TX power / duty cycle
   and use only the seconds needed.
4. **Record** - Save before/after logs to `reports/` (git-ignored). Never
   capture or store third-party traffic.
5. **Cleanup** - Restore placeholder SSIDs (`lab-*`), MACs (`00:11:22:33:44:55`),
   example.com / RFC5737 addresses, and clear any captured data from the device.

> Jammer / spoofer / replay projects are **proofs for study and simulation**
> only. They refuse live interference scenarios: a live bench trigger requires
> the `LAB_*` allowlist environment variable AND explicit `--yes` confirmation,
> and even then only against your own hardware in a shielded bench.

## Metrics

| Metric | Target | Where |
|---|---|---|
| Firmware compile | `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/h4_mousejack` PASS | CI/local |
| Host helper | `python3 host/h4_cli.py --demo` exits 0 (offline) | host/ |
| Unit tests | `python3 -m unittest discover -s tests` passes | tests/ |
| py_compile | every `host/*.py` compiles clean | CI/local |

## License

MIT

## Legal Disclaimer

## IMPORTANT: Read before use.

This project is provided for **educational and authorized security testing purposes only**. 

### Authorization Requirements
- You MUST have explicit written permission from the network owner before using this tool
- Unauthorized interception of network communications is illegal under federal and state laws
- This tool should ONLY be used on networks you own or have written authorization to test

### Legal Framework
- **Computer Fraud and Abuse Act (CFAA)**: Unauthorized access to computer systems is a federal crime
- **Wiretap Act (18 U.S.C. § 2511)**: Interception of electronic communications without consent is illegal
- **State Laws**: Many states have additional computer crime and wiretapping statutes
- **GDPR/CCPA**: Data collection may be subject to privacy regulations

### Acceptable Use
- Testing security of your own networks
- Authorized penetration testing with written scope
- Academic research in controlled lab environments
- Security education and training

### Prohibited Use
- Intercepting communications on networks you do not own
- Attacking infrastructure without authorization
- Any activity that violates applicable laws or regulations
- Commercial use without proper licensing

### No Warranty
This software is provided "AS IS" without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

### Responsible Disclosure
If you discover vulnerabilities using this tool, follow responsible disclosure practices:
1. Report to the vendor/owner privately
2. Allow reasonable time for remediation
3. Do not exploit beyond proof of concept
