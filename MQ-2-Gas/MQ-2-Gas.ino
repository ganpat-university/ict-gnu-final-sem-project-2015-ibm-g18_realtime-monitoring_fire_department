#include <WiFi.h>

#define MQ2_ANALOG_PIN  34  // A0 pin of MQ2 connected to GPIO 34 (for detailed readings)
#define MQ2_DIGITAL_PIN 18  // D0 pin of MQ2 connected to GPIO 23 (for quick detection)
#define BUZZER_PIN 22       // GPIO for Passive Buzzer
#define GAS_THRESHOLD 1800  // Analog threshold for gas detection (adjust as needed)

// Wi-Fi credentials
const char* ssid = "Airtel_E-44";
const char* password = "Orchid_E-44";

bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

void setup() {
    Serial.begin(115200);

    // Initialize MQ2 Sensor and Buzzer Pins
    pinMode(MQ2_ANALOG_PIN, INPUT);
    pinMode(MQ2_DIGITAL_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN); // Ensure buzzer is OFF initially

    // Connect to Wi-Fi
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    int gasAnalogValue = analogRead(MQ2_ANALOG_PIN);  // Read analog value
    int gasDigitalValue = digitalRead(MQ2_DIGITAL_PIN); // Read digital value

    Serial.print("MQ2 Analog: ");
    Serial.print(gasAnalogValue);
    Serial.print(" | Digital (D0): ");
    Serial.println(gasDigitalValue);

    // Debugging: Check if D0 is always LOW (faulty sensor)
    if (gasDigitalValue == LOW) {
        Serial.println("⚠️ D0 is LOW. Checking A0 value...");
    }

    // Detect Gas: Both conditions must be true
    if ((gasDigitalValue == LOW && gasAnalogValue > GAS_THRESHOLD) && !buzzerActive) {
        Serial.println("🚨 Gas detected! Activating buzzer...");
        buzzerActive = true;
        buzzerStartTime = millis();
        tone(BUZZER_PIN, 1000);  // Start buzzing at 1kHz
    }

    // Keep buzzer on for 10 seconds, then turn off
    if (buzzerActive && millis() - buzzerStartTime >= 10000) {
        noTone(BUZZER_PIN);  // Stop buzzer
        Serial.println("🔇 Buzzer OFF");
        buzzerActive = false;
    }

    delay(500);  // Check gas levels every 500ms
}




// void setup() {
//     Serial.begin(115200);
//     pinMode(23, INPUT);
// }

// void loop() {
//     Serial.print("D0 Value: ");
//     Serial.println(digitalRead(23));
//     delay(1000);
// }

// #define MQ2_ANALOG_PIN  34  // Try GPIO34 instead of 23
// void setup() {
//     Serial.begin(115200);
//     pinMode(MQ2_ANALOG_PIN, INPUT);
// }

// void loop() {
//     int gasValue = analogRead(MQ2_ANALOG_PIN);
//     Serial.print("Analog Value: ");
//     Serial.println(gasValue);
//     delay(1000);
// }
