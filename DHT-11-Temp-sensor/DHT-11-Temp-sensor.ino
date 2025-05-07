#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "Airtel_E-44";
const char* password = "Orchid_E-44";

// ThingSpeak API Details
const char* server = "http://api.thingspeak.com/update";
String apiKey = "PI1GYM30I1EGYUBE";  // Replace with your actual API Key

// DHT11 Sensor Setup
#define DHTPIN 23       // GPIO pin where DHT11 is connected
#define DHTTYPE DHT11  // DHT 11 Sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    delay(2000);
    float temperature = dht.readTemperature(); // Read temperature in Celsius
    float humidity = dht.readHumidity();      // Read humidity

    // Check if sensor reading is valid
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    sendDataToCloud(temperature, humidity);
    delay(5000);  // Send data every 10 seconds
}

void sendDataToCloud(float temperature, float humidity) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String requestURL = server;
        requestURL += "?api_key=" + apiKey;
        requestURL += "&field1=" + String(temperature);  // Send Temperature
        requestURL += "&field2=" + String(humidity);     // Send Humidity

        http.begin(requestURL);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("Data sent successfully! Response: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error sending data. HTTP response code: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi not connected. Unable to send data.");
    }
}
