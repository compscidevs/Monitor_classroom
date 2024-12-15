#define LED_PIN_MASK 0x01  // LED connected to PB0 (bit 0 of PORTB)
#define ADC_CHANNEL 0       // Use ADC0 (A0) for the photoresistor
#define LIGHT_THRESHOLD 200  // Light level threshold (0-1023)

void ADC_init() {
    // Set up the ADC: AVCC as reference voltage, right justified, using ADC0
    ADMUX = (1 << REFS0);  // Use AVCC as reference and select ADC0

    // Enable the ADC with a prescaler of 128
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Turn off digital input on ADC0 to save power
    DIDR0 = (1 << ADC0D);
}

uint16_t ADC_read() {
    // Start the ADC conversion
    ADCSRA |= (1 << ADSC);

    // Wait until the conversion is done
    while (ADCSRA & (1 << ADSC));

    // Return the result from the ADC (10-bit value)
    return ADC; // Ensure this reads from the correct register for your MCU
}

void setup() {
    // Configure LED_PIN (PB0) as an output
    volatile uint8_t* portDDRB = (uint8_t*)0x24;  // DDRB register for data direction
    *portDDRB |= LED_PIN_MASK; // Set PB0 as output

    ADC_init();                // Initialize the ADC
    Serial.begin(9600);        // Start Serial communication for debugging
}

void loop() {
    volatile uint8_t* portPORTB = (uint8_t*)0x25;  // PORTB register for output control
    volatile uint8_t* portPINB = (uint8_t*)0x23;   // PINB register for reading pin states

    uint16_t lightLevel = ADC_read();  // Read light level from photoresistor

    // If it's dark (light level is below the threshold)
    if (lightLevel < LIGHT_THRESHOLD) {
        *portPORTB |= LED_PIN_MASK;  // Turn on the LED (set PB0 HIGH)
    } else {
        *portPORTB &= ~LED_PIN_MASK; // Turn off the LED (set PB0 LOW)
    }

    delay(1000);  // Delay for 1 second to reduce flickering

    // Read light status (1 if ON, 0 if OFF) from PINB
    int lightStatus = (*portPINB & LED_PIN_MASK) ? 1 : 0;

    // Send light status via Serial
    Serial.println(lightStatus);

    delay(500);   // Delay before next loop iteration
}
