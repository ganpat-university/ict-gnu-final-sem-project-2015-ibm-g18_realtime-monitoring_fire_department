#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Define the GPS Serial Port (UART2)
HardwareSerial GPS_Serial(2); // Use UART2 (RX: GPIO16, TX: GPIO17)

// Create a TinyGPS++ object
TinyGPSPlus gps;

void setup() {
    Serial.begin(115200);  // Serial Monitor
    GPS_Serial.begin(9600, SERIAL_8N1, 16, 17); // GPS Module (Baud: 9600, RX: 16, TX: 17)

    Serial.println("📡 Waiting for GPS signal...");
}

void loop() {
    while (GPS_Serial.available() > 0) {
        gps.encode(GPS_Serial.read()); // Parse incoming GPS data
    }

    if (gps.location.isUpdated()) { // New GPS data available
        Serial.print("📍 Latitude: "); Serial.print(gps.location.lat(), 6);
        Serial.print(" | Longitude: "); Serial.println(gps.location.lng(), 6);
    }

    if (gps.altitude.isUpdated()) {
        Serial.print("⛰ Altitude: "); Serial.print(gps.altitude.meters());
        Serial.println(" meters");
    }

    if (gps.speed.isUpdated()) {
        Serial.print("🚗 Speed: "); Serial.print(gps.speed.kmph());
        Serial.println(" km/h");
    }

    if (gps.time.isUpdated()) {
        Serial.print("⏰ Time: "); 
        Serial.print(gps.time.hour()); Serial.print(":");
        Serial.print(gps.time.minute()); Serial.print(":");
        Serial.println(gps.time.second());
    }

    delay(1000); // Wait before reading again
}
