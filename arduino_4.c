struct EnergyUsage {
    uint8_t fanStatus;      // 1 means fan is ON, 0 means OFF
    uint8_t lightStatus;    // 1 means light is ON, 0 means OFF
    uint16_t fanDuration;   // Duration the fan has been ON (in minutes)
    uint16_t lightDuration; // Duration the light has been ON (in minutes)
    uint16_t timestamp;     // Time tracking (in minutes)
};

EnergyUsage energyData; // Create an instance of the EnergyUsage structure

void setup() {
    Serial.begin(9600); // Start Serial communication
    Serial.println("Master setup complete");

    // Set initial values for the energy data
    energyData.fanStatus = 0;
    energyData.lightStatus = 0;
    energyData.fanDuration = 0;
    energyData.lightDuration = 0;
    energyData.timestamp = 0;
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

    String key = data.substring(0, delimiterIndex); // Get the key part of the data
    String value = data.substring(delimiterIndex + 1); // Get the value part

    if (key == "1") {
        energyData.fanStatus = value.toInt(); // Update fan status based on input
    } else if (key == "3") {
        energyData.lightStatus = (value == "ON") ? 1 : 0; // Update light status
    } else {
        Serial.println("Unknown key, skipping...");
    }

    energyData.timestamp++; // Increment timestamp for each update
    if (energyData.fanStatus) energyData.fanDuration++; // Increase fan duration if it's ON
    if (energyData.lightStatus) energyData.lightDuration++; // Increase light duration if it's ON
}

void saveToEEPROM(int startAddress, EnergyUsage usage) {
    // Write fan status
    EEARL = startAddress; // Set low address
    EEARH = 0;            // Set high address (0 for 1KB EEPROM)
    EEDR = usage.fanStatus; // Load data into EEDR
    EEMPE = 1;            // Enable master write
    EEPE = 1;             // Start write operation

    // Write light status
    EEARL = startAddress + 1;
    EEDR = usage.lightStatus;
    EEMPE = 1;
    EEPE = 1;

    // Write fan duration
    EEARL = startAddress + 2;
    EEDR = usage.fanDuration & 0xFF; // Low byte
    EEMPE = 1;
    EEPE = 1;

    EEARL = startAddress + 3;
    EEDR = (usage.fanDuration >> 8) & 0xFF; // High byte
    EEMPE = 1;
    EEPE = 1;

    // Write light duration
    EEARL = startAddress + 4;
    EEDR = usage.lightDuration & 0xFF; // Low byte
    EEMPE = 1;
    EEPE = 1;

    EEARL = startAddress + 5;
    EEDR = (usage.lightDuration >> 8) & 0xFF; // High byte
    EEMPE = 1;
    EEPE = 1;

    // Write timestamp
    EEARL = startAddress + 6;
    EEDR = usage.timestamp & 0xFF; // Low byte
    EEMPE = 1;
    EEPE = 1;

    EEARL = startAddress + 7;
    EEDR = (usage.timestamp >> 8) & 0xFF; // High byte
    EEMPE = 1;
    EEPE = 1;
}
EnergyUsage readFromEEPROM(int startAddress) {
    EnergyUsage usage;

    // Read fan status
    EEARL = startAddress;
    EERE = 1; // Trigger read
    usage.fanStatus = EEDR;

    // Read light status
    EEARL++;
    EERE = 1;
    usage.lightStatus = EEDR;

    // Read fan duration
    EEARL += 2;
    EERE = 1;
    usage.fanDuration = EEDR; // Low byte

    EEARL++;
    EERE = 1;
    usage.fanDuration |= (EEDR << 8); // High byte

    // Read light duration
    EEARL += 2;
    EERE = 1;
    usage.lightDuration = EEDR; // Low byte

    EEARL++;
    EERE = 1;
    usage.lightDuration |= (EEDR << 8); // High byte

    // Read timestamp
    EEARL += 2;
    EERE = 1;
    usage.timestamp = EEDR; // Low byte

    EEARL++;
    EERE = 1;
    usage.timestamp |= (EEDR << 8); // High byte

    return usage; // Return populated structure
}
void printEnergyUsage(EnergyUsage usage) {
    Serial.println("----- Energy Usage Data -----");
    
    Serial.print("Fan Status: ");
    Serial.println(usage.fanStatus ? "ON" : "OFF"); // Print fan status

    Serial.print("Light Status: ");
    Serial.println(usage.lightStatus ? "ON" : "OFF"); // Print light status

    Serial.print("Fan Duration (min): ");
    Serial.println(usage.fanDuration); // Print how long the fan has been ON

    Serial.print("Light Duration (min): ");
    Serial.println(usage.lightDuration); // Print how long the light has been ON

    Serial.print("Timestamp (min): ");
    Serial.println(usage.timestamp); // Print current timestamp
    
    Serial.println("-----------------------------");
}
