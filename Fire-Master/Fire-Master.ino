#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>
#include <secrets.h>
#include <NTPClient.h>

WiFi credentials
const char* WIFI_SSID = "Airtel_E-44";
const char* WIFI_PASSWORD = "Orchid_E-44";
NTP Client Setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // IST (UTC+5:30)
// AWS IoT Core credentials
const char* THINGNAME = "ESP-32";
const char* MQTT_HOST = "a29b4bhpnjvyuk-ats.iot.ap-south-1.amazonaws.com";

// MQTT topics
const char* AWS_IOT_PUBLISH_TOPIC = "esp32/pub";
const char* AWS_IOT_SUBSCRIBE_TOPIC = "esp32/sub";

// LM35 Sensor Pin
#define LM35_PIN 35  // Temperature sensor pin
#define FLAME_SENSOR_PIN 36  // Flame sensor pin
#define MQ2_ANALOG_PIN 34  // MQ-2 analog pin
#define BUZZER_PIN 22  // Passive buzzer pin

// Thresholds
#define TEMP_THRESHOLD 10.0  // Temperature threshold in °C
#define GAS_THRESHOLD 1800  // MQ-2 analog threshold
#define FLAME_DETECTED LOW  // Flame sensor active state

// Interval for publishing data (5 seconds)
const long interval = 5000;
unsigned long lastMillis = 0;

// Variables for buzzer control
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

// Secure WiFi client
WiFiClientSecure net;
PubSubClient client(net);

// AWS IoT Certificates (Replace these with your actual certs)


// const char* CLIENT_CERT = R"EOF(


// const char* PRIVATE_KEY = R"EOF(

// NTP server details
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;  // GMT+5:30 (Indian Standard Time)
const int daylightOffset_sec = 0;

// Function to connect to WiFi
void connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 15) {
        Serial.print(".");
        delay(1000);
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed. Restarting...");
        ESP.restart();
    }
}

// Function to connect to AWS IoT Core
void connectAWS() {
    Serial.println("Connecting to AWS IoT...");
    net.setCACert(ROOT_CA);
    net.setCertificate(CLIENT_CERT);
    net.setPrivateKey(PRIVATE_KEY);

    client.setServer(MQTT_HOST, 8883);

    int attempt = 0;
    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        Serial.println(" Connection failed, reason: " + String(client.state()));
        delay(2000);
        attempt++;
        if (attempt > 5) {
            Serial.println("Restarting ESP32...");
            ESP.restart();
        }
    }
    Serial.println("\nConnected to AWS IoT!");
}

// Function to read temperature from LM35
float readTemperature() {
    int adcValue = analogRead(LM35_PIN);
    float voltage = adcValue * (3.3 / 4095.0);  // Convert ADC value to voltage
    return voltage * 100.0;  // LM35 conversion to temperature
}

// Function to publish data to AWS IoT Core
// void publishData(float temperature, int flameSensorValue, int gasAnalogValue) {
//     char payload[250];
//     unsigned long epochTime = millis() / 1000;  // Get current time in seconds
//     snprintf(payload, sizeof(payload),
//              "{\"temperature\": %.2f, \"flame\": %d, \"gasAnalog\": %d, \"timeStamp\": %lu}",
//              temperature, flameSensorValue, gasAnalogValue, epochTime);
//     Serial.println("Publishing: " + String(payload));
//     client.publish(AWS_IOT_PUBLISH_TOPIC, payload);
// }
// Function to publish data to AWS IoT Core
void publishData(float temperature, int flameSensorValue, int gasAnalogValue) {
    timeClient.update();  // Ensure the time is updated
    long epochTime = timeClient.getEpochTime(); // Get correct timestamp
    
    char payload[250];
    snprintf(payload, sizeof(payload),
             "{\"device_id\": \"%s\", \"temperature\": %.2f, \"flame\": %d, \"gasAnalog\": %d, \"timestamp\": %ld}",
             THINGNAME, temperature, flameSensorValue, gasAnalogValue, epochTime);
    
    Serial.println("Publishing: " + String(payload));
    
    if (client.publish(AWS_IOT_PUBLISH_TOPIC, payload)) {
        Serial.println("✅ Data published successfully!");
    } else {
        Serial.println("❌ Failed to publish data.");
    }
}


void setup() {
    Serial.begin(115200);
    pinMode(LM35_PIN, INPUT);
    pinMode(FLAME_SENSOR_PIN, INPUT_PULLUP);
    pinMode(MQ2_ANALOG_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);
    connectWiFi();
    connectAWS();
}

void loop() {
    if (!client.connected()) {
        connectAWS();
    }
    client.loop();

    // Read sensor values
    float temperature = readTemperature();
    int flameSensorValue = digitalRead(FLAME_SENSOR_PIN);
    int gasAnalogValue = analogRead(MQ2_ANALOG_PIN);

    // Log sensor values
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Flame: ");
    Serial.print(flameSensorValue == FLAME_DETECTED ? "Detected" : "Not Detected");
    Serial.print(" | MQ2 Analog: ");
    Serial.println(gasAnalogValue);

    // Check if all conditions are met to activate the buzzer
    if (!buzzerActive && flameSensorValue == FLAME_DETECTED && temperature > TEMP_THRESHOLD && gasAnalogValue > GAS_THRESHOLD) {
        Serial.println("🚨 All conditions met! Activating buzzer...");
        buzzerActive = true;
        buzzerStartTime = millis();
        tone(BUZZER_PIN, 3000);  // Start buzzing at 1kHz
    }

    // Turn off the buzzer after 10 seconds
    if (buzzerActive && millis() - buzzerStartTime >= 10000) {
        noTone(BUZZER_PIN);  // Stop the buzzer
        Serial.println("🔇 Buzzer OFF");
        buzzerActive = false;
    }

    // Publish data to AWS IoT Core every 5 seconds
    if (millis() - lastMillis > interval) {
        lastMillis = millis();
        publishData(temperature, flameSensorValue, gasAnalogValue);
    }

    // Small delay to avoid excessive CPU usage
    delay(2000);
}