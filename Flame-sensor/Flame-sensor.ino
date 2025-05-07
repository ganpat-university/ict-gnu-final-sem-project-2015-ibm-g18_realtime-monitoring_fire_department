#include <WiFi.h>

#define FLAME_SENSOR_PIN 36  // GPIO for Flame Sensor
#define BUZZER_PIN 22        // GPIO for Passive Buzzer

// Wi-Fi credentials
const char* ssid = "Airtel_E-44";
const char* password = "Orchid_E-44";

bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

void setup() {
    Serial.begin(115200);

    // Initialize Flame Sensor Pin
    pinMode(FLAME_SENSOR_PIN, INPUT_PULLUP);

    // Initialize Buzzer Pin
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
    int flameSensorValue = digitalRead(FLAME_SENSOR_PIN);

    if (flameSensorValue == LOW && !buzzerActive) { 
        Serial.println("🔥 Flame detected! 🔥");
        buzzerActive = true;
        buzzerStartTime = millis();
        tone(BUZZER_PIN, 1000);  // Start buzzing
    }

    // Keep the buzzer buzzing for 10 seconds, even if flame disappears
    if (buzzerActive && millis() - buzzerStartTime >= 10000) {
        noTone(BUZZER_PIN);  // Stop the buzzer after 10 seconds
        Serial.println("🔇 Buzzer OFF");
        buzzerActive = false;
    }

    Serial.println(flameSensorValue == LOW ? "🔥 Fire Still Detected!" : "✅ No Fire");

    delay(500); // Check sensor every 500ms
}
