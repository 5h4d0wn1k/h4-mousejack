#!/usr/bin/env python3
"""H4 - nRF24 Protocol Hacker host helper: offline HID payload classification.
Educational/authorized own-lab use only (see README "IMPORTANT").
"""
import argparse
import os
import sys

MOD = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, MOD)
from hw_common import DEMO_TAG, read_target


def classify(payload):
    """Classify an nRF24 HID report. Payload is a bytes object."""
    if len(payload) == 3:
        btns = payload[0]
        return "MOUSE buttons=0x%02X dx=%d dy=%d" % (btns, payload[1], payload[2])
    if len(payload) == 8:
        keys = [k for k in payload[2:] if k != 0]
        return "KEYBOARD mod=0x%02X keys=%s" % (payload[0], ",".join("%02X" % k for k in keys))
    return "UNKNOWN (%d bytes)" % len(payload)


def analyze(text):
    reports = []
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        try:
            b = bytes.fromhex(line)
        except ValueError:
            continue
        reports.append({"payload": b, "summary": classify(b)})
    return reports


def run_demo():
    print("=== H4 nRF24 HID report classification (offline) ===")
    text = read_target("fixtures/nrf24_capture.log",
                       "01 0A 00\n01 F4 01\n00 00 04 00 00 00 00 00\nAA BB CC\n")
    for r in analyze(text):
        print("  len=%d  %s" % (len(r["payload"]), r["summary"]))
    print(DEMO_TAG)
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(
        description="H4 MouseJack - offline nRF24 HID classification")
    p.add_argument("--demo", action="store_true", help="offline demo (exit 0)")
    p.add_argument("--file", help="payload capture path")
    args = p.parse_args(argv)
    if args.demo or not args.file:
        return run_demo()
    text = open(args.file).read()
    for r in analyze(text):
        print(r["summary"])
    return 0


if __name__ == "__main__":
    sys.exit(main())
