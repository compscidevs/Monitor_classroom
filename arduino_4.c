// Structure Definition
struct EnergyUsage {
    uint8_t fanStatus;      // 1 means fan is ON, 0 means OFF
    uint8_t lightStatus;    // 1 means light is ON, 0 means OFF
    uint16_t fanDuration;   // Duration the fan has been ON (in minutes)
    uint16_t lightDuration; // Duration the light has been ON (in minutes)
    uint16_t timestamp;     // Time tracking (in minutes)
};

// Function Prototypes
void parseAndUpdateEnergyUsage(String data);
void saveToEEPROM(int startAddress, EnergyUsage usage);
EnergyUsage readFromEEPROM(int startAddress);
void writeEEPROM(int address, uint8_t data);
uint8_t readEEPROM(int address);
void printEnergyUsage(EnergyUsage usage);

// Global variable for Energy Data
EnergyUsage energyData;

void setup() {
    Serial.begin(9600); // Start Serial communication
    Serial.println("Master setup complete");

    // Initialize energy data with default values
    energyData.fanStatus = 0;
    energyData.lightStatus = 0;
    energyData.fanDuration = 0;
    energyData.lightDuration = 0;
    energyData.timestamp = 0;

    // Load any previously saved data from EEPROM
    energyData = readFromEEPROM(0);
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // Read incoming data line
        Serial.print("Received: ");
        Serial.println(data);

        // Update the energy usage data based on the received input
        parseAndUpdateEnergyUsage(data);

        // Save updated data to EEPROM starting at address 0
        saveToEEPROM(0, energyData);

        // Print the current energy usage data for verification
        printEnergyUsage(energyData);
    }

    delay(50); // Small delay for stable serial communication
}

void parseAndUpdateEnergyUsage(String data) {
    int delimiterIndex = data.indexOf(':'); // Find the position of the delimiter
    if (delimiterIndex == -1) {
        Serial.println("Invalid data format, skipping...");
        return; // Exit if format is incorrect
    }

    String key = data.substring(0, delimiterIndex); // Extract key part
    String value = data.substring(delimiterIndex + 1); // Extract value part

    if (key == "2") { // Update fan status
        energyData.fanStatus = value.toInt();
    } else if (key == "3") { // Update light status
        energyData.lightStatus = value.toInt();
    } else {
        Serial.println("Unknown key, skipping...");
    }

    // Increment durations and timestamp
    energyData.timestamp++;
    if (energyData.fanStatus) energyData.fanDuration++;
    if (energyData.lightStatus) energyData.lightDuration++;
}

void saveToEEPROM(int startAddress, EnergyUsage usage) {
    writeEEPROM(startAddress, usage.fanStatus);           // Fan Status
    writeEEPROM(startAddress + 1, usage.lightStatus);     // Light Status

    writeEEPROM(startAddress + 2, usage.fanDuration & 0xFF);       // Fan Duration Low Byte
    writeEEPROM(startAddress + 3, (usage.fanDuration >> 8) & 0xFF);// Fan Duration High Byte

    writeEEPROM(startAddress + 4, usage.lightDuration & 0xFF);     // Light Duration Low Byte
    writeEEPROM(startAddress + 5, (usage.lightDuration >> 8) & 0xFF);// Light Duration High Byte

    writeEEPROM(startAddress + 6, usage.timestamp & 0xFF);         // Timestamp Low Byte
    writeEEPROM(startAddress + 7, (usage.timestamp >> 8) & 0xFF);  // Timestamp High Byte
}

EnergyUsage readFromEEPROM(int startAddress) {
    EnergyUsage usage;

    usage.fanStatus = readEEPROM(startAddress);               // Fan Status
    usage.lightStatus = readEEPROM(startAddress + 1);         // Light Status

    usage.fanDuration = readEEPROM(startAddress + 2);         // Fan Duration Low Byte
    usage.fanDuration |= (readEEPROM(startAddress + 3) << 8); // Fan Duration High Byte

    usage.lightDuration = readEEPROM(startAddress + 4);       // Light Duration Low Byte
    usage.lightDuration |= (readEEPROM(startAddress + 5) << 8); // Light Duration High Byte

    usage.timestamp = readEEPROM(startAddress + 6);           // Timestamp Low Byte
    usage.timestamp |= (readEEPROM(startAddress + 7) << 8);   // Timestamp High Byte

    return usage;
}

void writeEEPROM(int address, uint8_t data) {
    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address;            // Set EEPROM address
    EEDR = data;               // Set EEPROM data register
    EECR |= (1 << EEMPE);      // Master Write Enable
    EECR |= (1 << EEPE);       // Start EEPROM write
}

uint8_t readEEPROM(int address) {
    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address;            // Set EEPROM address
    EECR |= (1 << EERE);       // Start EEPROM read
    return EEDR;               // Return data from data register
}

void printEnergyUsage(EnergyUsage usage) {
    Serial.println("----- Energy Usage Data -----");

    Serial.print("Fan Status: ");
    Serial.println(usage.fanStatus ? "ON" : "OFF"); // Print fan status

    Serial.print("Light Status: ");
    Serial.println(usage.lightStatus ? "ON" : "OFF"); // Print light status

    Serial.print("Fan Duration (min): ");
    Serial.println(usage.fanDuration); // Print fan ON duration

    Serial.print("Light Duration (min): ");
    Serial.println(usage.lightDuration); // Print light ON duration

    Serial.print("Timestamp (min): ");
    Serial.println(usage.timestamp); // Print timestamp

    Serial.println("-----------------------------");
}
