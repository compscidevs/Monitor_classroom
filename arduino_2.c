// C++ code for AVR registers
// Define bit mask for Rx (D0)
#define RX_MASK 0x01    // 0000 0001 (Pin 0 of Port D)
#define ADC_CHANNEL 0
#define TEMP_THRESHOLD 200
#define BIT5_MASK 0b00100000 // Motor control on PB5

  // For DC motor
volatile unsigned char *portDDRB = (unsigned char *)0x24;  // Data Direction Register for Port B
volatile unsigned char *portDataB = (unsigned char *)0x25; // Data Register for Port B
volatile unsigned char *portPinB = (unsigned char *)0x23;  // Input Pins Register for Port B


// Function to initialize ADC
void ADC_init() {
    ADMUX = (1 << REFS0);  // AVCC as reference voltage, ADC0 selected (default)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = (1 << ADC0D); // Disable digital input on ADC0 to save power
}

// Function to read from ADC
uint16_t ADC_read() {
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADC; // Return ADC result
}

void setup() {
  ADC_init();
  // For DC motor
volatile unsigned char *portDDRB = (unsigned char *)0x24;  // Data Direction Register for Port B
volatile unsigned char *portDataB = (unsigned char *)0x25; // Data Register for Port B
volatile unsigned char *portPinB = (unsigned char *)0x23;  // Input Pins Register for Port B

  // Seetting up pin 13 as output
  
  *portDDRB |= BIT5_MASK;       // Set PB5 as output
  *portDataB &= ~BIT5_MASK;     // Ensure motor is off initially
  
  // Define pointers to registers
  volatile uint8_t* portDDRD = (volatile uint8_t*)0x2A;  // DDRD register
  volatile uint8_t* portPORTD = (volatile uint8_t*)0x2B; // PORTD register
  volatile uint8_t* portPIND = (volatile uint8_t*)0x29;  // PIND register (to read data)

  // Configure Rx (D0) as INPUT
  *portDDRD &= ~RX_MASK; // Clear bit 0 in DDRD to set as input

  // Disable pull-up resistor on Rx (write 0 to bit 0 in PORTD)
  *portPORTD &= ~RX_MASK; // Clear bit 0 in PORTD to avoid enabling pull-up resistor

  // Initialize Serial communication for debugging (optional)
  //sensor1.begin(9600); // Communication with Arduino 1
  Serial.begin(9600);
  //Serial.begin(9600);  // Communication with Arduino 4
    //Serial1.begin(9600); // Communication with Arduino 1
}

void loop() {
  int receivedValue = 0;
  
  if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n'); // Read data from Arduino 1

        // Parse the data
        if (data.startsWith("13:")) {
            // Data from Arduino 3
            Serial.print("3: ");   // Forward to Arduino 4 with ID for Arduino 3
            Serial.println(data.substring(4)); // Remove "13: " prefix
        } else {
            // Flag from Arduino 1
          delay(50);
          receivedValue = Serial.parseInt();
            Serial.print("1: ");   // Forward to Arduino 4 with ID for Arduino 1
            Serial.println(receivedValue); // Send the flag value
        }
    }
  delay(50); // Adjust based on your requirements
  // Define pointer to PIND (read input value)
  volatile uint8_t* portPIND = (volatile uint8_t*)0x29; // PIND register

  // Read the value from Rx (D0)
  //bool receivedValue = (*portPIND & RX_MASK); // Mask other bits to isolate D0

  //Serial.println(receivedValue);
  
  int readValue = ADC_read();

  if (readValue > TEMP_THRESHOLD && receivedValue){
    *portDataB |= BIT5_MASK; // Turn the motor on
  }else{
    *portDataB &= ~BIT5_MASK; // Turn the motor off

  }
  
  //Serial.println(readValue); //printing the temperature
  
  //Serial Transfer to Master
  //SerialComm();
  delay(200); // Wait for 50ms before reading again
  Serial.print("2:"); // Device ID = 2
  Serial.println(readValue); // Send temperature reading
  
  delay(100);
  

  
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
