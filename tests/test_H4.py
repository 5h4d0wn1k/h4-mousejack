import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
import h4_cli as m


class TestClassify(unittest.TestCase):
    def test_mouse_report(self):
        s = m.classify(bytes([0x01, 0x0A, 0x00]))
        self.assertIn("MOUSE", s)
        self.assertIn("0x01", s)

    def test_keyboard_report(self):
        s = m.classify(bytes([0x00, 0x00, 0x04, 0, 0, 0, 0, 0]))
        self.assertIn("KEYBOARD", s)
        self.assertIn("04", s)

    def test_unknown(self):
        self.assertIn("UNKNOWN", m.classify(bytes([0xDE, 0xAD])))


if __name__ == "__main__":
    unittest.main()
