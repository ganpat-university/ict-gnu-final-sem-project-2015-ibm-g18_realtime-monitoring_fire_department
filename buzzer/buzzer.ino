#define BUZZER_PIN 23  // Change this to the pin where your buzzer is connected

void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    tone(BUZZER_PIN, 1000);  // Send a 1kHz frequency tone
    delay(1000);
    noTone(BUZZER_PIN);  // Stop the tone
    delay(1000);
}
