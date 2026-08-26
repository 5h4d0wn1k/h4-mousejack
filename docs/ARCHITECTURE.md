# H4 — nRF24 Protocol Hacker Architecture

## System Overview

The H4 MouseJack tool exploits the fact that many wireless keyboards and mice use the nRF24L01+ radio without encryption. This allows:
1. **Sniffing**: Capture keystrokes and mouse movements
2. **Injection**: Send fake keystrokes and mouse movements
3. **Device Tracking**: Identify and monitor wireless peripherals

## Attack Flow

```
┌─────────────────┐
│  Channel Scan   │
│  (41 channels)  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Traffic Detect │
│  (nRF24 radio)  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  HID Parsing    │
│  (Mouse/Keys)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Device Track   │
│  (Address/IP)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Injection      │
│  (Spoofed HID)  │
└─────────────────┘
```

## nRF24L01+ Protocol

### Packet Structure

```
Preamble (1-2 bytes)
Address (3-5 bytes)
Packet Control (9 bits)
Payload (0-32 bytes)
CRC (0-2 bytes)
```

### Channel Hopping

Wireless peripherals hop channels to avoid interference:
- 41 channels (2400-2441 MHz)
- Typical hop rate: 50-100 hops/second
- Hopping pattern: Device-specific

## HID Report Analysis

### Mouse Reports

```
Byte 0: Buttons
  Bit 0: Left
  Bit 1: Right
  Bit 2: Middle
  Bits 3-7: Reserved

Byte 1: X-axis (signed 8-bit)
Byte 2: Y-axis (signed 8-bit)
```

### Keyboard Reports

```
Byte 0: Modifier keys
  Bit 0: Left Ctrl
  Bit 1: Left Shift
  Bit 2: Left Alt
  Bit 3: Left GUI
  Bits 4-7: Right modifiers

Byte 1: Reserved

Bytes 2-7: Key codes (up to 6 simultaneous keys)
```

## Device Identification

### By Packet Size

| Size | Device Type |
|------|-------------|
| 3 bytes | Mouse |
| 8 bytes | Keyboard |
| 10 bytes | Extended keyboard (media keys) |
| Other | Unknown |

### By Channel

| Channel | Common Vendor |
|---------|---------------|
| 8 | Logitech Unifying |
| 10 | Dell |
| 19 | Microsoft |
| 25 | Apple (partial) |
| 37 | Various |

## Injection Techniques

### Keystroke Injection

1. Construct HID keyboard report
2. Set modifier byte (Ctrl, Shift, etc.)
3. Set key code in bytes 2-7
4. Send packet on target channel
5. Send release packet (all zeros)

### Mouse Movement Injection

1. Construct HID mouse report
2. Set button byte (0 for movement only)
3. Set X/Y displacement (signed 8-bit)
4. Send packet on target channel

### Timing Considerations

- Send rate: Limited by device polling rate
- Typical mouse: 125-1000 Hz
- Typical keyboard: 125-250 Hz
- Too fast: Device may ignore packets

## Defensive Countermeasures

1. **Encryption**: Encrypt HID traffic (rare in consumer devices)
2. **Authentication**: Pair devices with unique keys
3. **Frequency Hopping**: Randomized hop patterns
4. **Signal Detection**: Monitor for unusual 2.4 GHz activity
5. **Wired Peripherals**: Avoid wireless for sensitive use

## Research Applications

### P2 — ESP-NOW Security SoK

H4 provides data on wireless peripheral security:
- Device populations
- Protocol usage
- Encryption prevalence

### H14 — RF Lockpick

Extend nRF24 analysis to other 2.4 GHz protocols:
- Zigbee (partial)
- Thread (partial)
- Proprietary protocols

### W8 — Evil Portal

Combine with WiFi attacks for comprehensive wireless assessment.

## References

- MouseJack Research (Bastille Networks)
- nRF24L01+ Datasheet
- USB HID Specification
- Wireless Keyboard Security Analysis
