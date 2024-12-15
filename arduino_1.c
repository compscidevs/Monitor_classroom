
#define TX_MASK 0x02    // Bit for Tx (D1) on Port D
#define BIT0_MASK 0x01  // Bit for A0 on Port C

volatile uint32_t timerCounter = 0; // Counts timer ticks

void setup() {
    // Set up pointers to the hardware registers
    volatile uint8_t* portDDRC = (volatile uint8_t*)0x27;  // DDRC for data direction of Port C
    volatile uint8_t* portPORTC = (volatile uint8_t*)0x28; // PORTC for output on Port C
    volatile uint8_t* portDDRD = (volatile uint8_t*)0x2A;  // DDRD for data direction of Port D
    volatile uint8_t* portPORTD = (volatile uint8_t*)0x2B; // PORTD for output on Port D

    // Set D1 (Tx) as an output pin
    *portDDRD |= TX_MASK; // Configure Tx pin as output

    // Set A0 as an input pin
    *portDDRC &= ~BIT0_MASK; // Configure A0 pin as input

    // Turn on pull-up resistor for A0
    *portPORTC |= BIT0_MASK; // Enable pull-up resistor on A0

    // Initialize Timer1
    setupTimer();

    // Start Serial communication for debugging (optional)
    Serial.begin(9600);
}

void loop() {
    // Set up pointers to read input and write output
    volatile uint8_t* portPINC = (volatile uint8_t*)0x26;  // PINC for reading input values
    volatile uint8_t* portPORTD = (volatile uint8_t*)0x2B; // PORTD for writing output values

    // Read the value from A0
    uint8_t pinValue = *portPINC & BIT0_MASK; // Get the value of A0

    // If A0 is high, set Tx (D1) high
    if (pinValue) {
        *portPORTD |= TX_MASK; // Turn Tx pin HIGH
    }

  Serial.print("1: ");  
  Serial.println(pinValue); // Print the value of A0
  //SerialComm();

    // Delay using custom function
    customDelay(1); // Custom delay
  if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // Read incoming data
        Serial.print("13: ");
        Serial.println(data);
    }
  customDelay(0.5); // Delay for 10 seconds
  	//Serial.println("delay executed"); // Indicate delay is done
}

// Function to set up Timer1
void setupTimer() {
    TCCR1A = 0;            // Reset Timer/Counter Control Register A
    TCCR1B = 0;            // Reset Timer/Counter Control Register B

    TCCR1B |= (1 << WGM12); // Set Timer1 to CTC mode (Clear Timer on Compare Match)
    OCR1A = 256 - 1;       // Set compare match value for a 16 µs interval

    TCCR1B |= (1 << CS10); // No prescaler, use full clock speed (16 MHz)

    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 Compare Match Interrupt
}

// Interrupt Service Routine for Timer1 Compare Match
ISR(TIMER1_COMPA_vect) {
    timerCounter++; // Increment the global counter
}

// Custom delay function
void customDelay(uint16_t seconds) {
    uint32_t intervals = (uint32_t)seconds * 1000000 / 16; // Calculate 16 µs intervals for the given seconds
    timerCounter = 0;                                      // Reset the timer counter
    while (timerCounter < intervals);                      // Wait until the required intervals have passed
}

void SerialComm(){
  if (Serial.available() > 0) {
        String request = Serial.readStringUntil('\n'); // Read poll request
        if (request == "POLL:1") {
            Serial.println("Motion:1"); // Send data for Arduino 1
        } else if (request == "POLL:2") {
            Serial.println("Temperature:24.5"); // Send data for Arduino 2
        } else if (request == "POLL:3") {
            Serial.println("Light:ON"); // Send data for Arduino 3
        }
    }
}
