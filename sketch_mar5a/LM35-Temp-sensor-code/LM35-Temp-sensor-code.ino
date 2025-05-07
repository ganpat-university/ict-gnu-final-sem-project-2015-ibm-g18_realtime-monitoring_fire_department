#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WiFi credentials
const char* WIFI_SSID = "Airtel_E-44";
const char* WIFI_PASSWORD = "Orchid_E-44";

// AWS IoT Core credentials
const char* THINGNAME = "ESP-32";
const char* MQTT_HOST = "a29b4bhpnjvyuk-ats.iot.ap-south-1.amazonaws.com";

// MQTT topics
const char* AWS_IOT_PUBLISH_TOPIC = "esp32/pub";
const char* AWS_IOT_SUBSCRIBE_TOPIC = "esp32/sub";

// LM35 Sensor Pin
#define LM35_PIN 35  // Change according to your ESP32 wiring

// Interval for publishing data (5 seconds)
const long interval = 5000;
unsigned long lastMillis = 0;

// Secure WiFi client
WiFiClientSecure net;
PubSubClient client(net);

// AWS IoT Certificates (Replace these with your actual certs)
const char* ROOT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* CLIENT_CERT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJDQh0i8ypOVi9r8wn76Cdlxsln+MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNTAzMjUxMjMw
NTBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHf+rAvPnRuMMhDBnD
vsPHMt16wqXegr3q40VyXS8ZsqLYohG7mPzqKOGI0WzdFn4VBf9vEnyD11z3+ttD
6cO9wq7ZtngxMd/HGq2X8n5YV+pIGL2Cf68KPf9+UuLYdfl1KSEAcCP8WrVzf6zA
u1s0A++2mSPjgzAmWD+rXHOSeMpyhgXTTJ010nPHb0yeDjCgDCLOpCZHDoLCIvEl
FAH3Buo5cDgcK1dpZ/OyEe+MyGXmgAOpqrHivxdCGS5xBy1z3orIjqbUY26oFmT3
xWXKFbaLU1tJEhctCDHR1Iurqfwh3Cy8tGzeKjj4mNKu8Cs3Hq5NneZoezPm0OKh
HYlzAgMBAAGjYDBeMB8GA1UdIwQYMBaAFOTbSN+MphLwB9mNjDx1FIpshhcKMB0G
A1UdDgQWBBQqn1u/9BoyTgUfLaZZAoKxcOBDfzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAJtcFOVMY1gMiIifz1+s+ktOE
Lj34Z4iaQpORXnCYkdTZwJHkINIl+0suxHP85zZgGN1HqQ9XQN+xRxGWEXXY7AMD
FaAozh1A43F5BK2sWdWMLyjavAzuMXHA4MT6o/OaiAtHdZ4gsEcpOTkKaqm/GNI9
mEtegOaKZSMsyYfBRQvGnfcmummAsxa/Q7CoYwhjZKyZV51kVOeRgMQZ89dbeoYl
NwwA41K4ifs5woT5DVPh4O8bA+D9jPUzQIpsMpsd0ZA7ykfa1SxySSkFAUbrAie8
CxiH3HP5+gWAn3rRlgoSkbTJvvdIgsFfBFgFp3BcULhFh6EVoP/t1Xory7F/cQ==
-----END CERTIFICATE-----
)EOF";

const char* PRIVATE_KEY = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAx3/qwLz50bjDIQwZw77DxzLdesKl3oK96uNFcl0vGbKi2KIR
u5j86ijhiNFs3RZ+FQX/bxJ8g9dc9/rbQ+nDvcKu2bZ4MTHfxxqtl/J+WFfqSBi9
gn+vCj3/flLi2HX5dSkhAHAj/Fq1c3+swLtbNAPvtpkj44MwJlg/q1xzknjKcoYF
00ydNdJzx29Mng4woAwizqQmRw6CwiLxJRQB9wbqOXA4HCtXaWfzshHvjMhl5oAD
qaqx4r8XQhkucQctc96KyI6m1GNuqBZk98VlyhW2i1NbSRIXLQgx0dSLq6n8Idws
vLRs3io4+JjSrvArNx6uTZ3maHsz5tDioR2JcwIDAQABAoIBABhwFvWdMIz8MQrv
TIip6ZmRJYhQJ7lmEpWfWXZ067weIcyfrcRpLTIR5+62mLtMcg1pOq0EerQ7wWuv
EwTyscOhAYIx0+9SyLK/FdyzG77+pfhG74/4zBxZE98JkDgr2276psxC8GWNRwTq
8uJ5EOcCRG79u2O3QkKwNLxtCss6QTRayeO52W7ak3L94IdIbenyIYXxmFMJDjhH
OfyupDYiwHAwUU6X9iYW1beocfeB1R/wSwvn+5P/aacqagkGATh2o6EjQZiRDurk
eidbVQhH+ZBIOVwURorHP6SHp8PsQ2viS+ren6AGw+EA5V2/jDa6TAwQr+m68M6Z
EFIda6ECgYEA8btIosTctPAfuEsplCmze5+0NDkcbyc4wtCEKgjnXRCwtcRdIUua
7ZLBoHcsqM/HeBeHGzeWcQX0mFGw5qif8YCDz14rcaCa+evQ6GprrQSGJayAlG5A
Xq0Bfg6EnvcYNtP1E0FWJ6O8XiM1v6yw+Ch/6FdY+HCFqBbm7i4lL4UCgYEA00Z7
j79cvMEKzGZrG+jHuifBkDEscTR0YuotDikJidVnuagzDq+dZnTpjVCjaCiqTMeH
au0zAp3wdEUZy/I9UzbVsNXzYjv9R5VCZFKAOwQoUPU3PJO4QQM4wOhz4qReg91Y
gEC9eS+CxtCkj6v91Fb9lavh0jZekcIUFQBfGpcCgYBkSEHV5L17ArLmDahuIu6m
tpZXo+m74xWVHFP37EGvRiFJBvFrwqW/MUbKfyYz0h76HyiZjfIHaWUTW7B7wDC6
S4pjGLoFJY+lZsL/6EZLPLk6H9onuaVSoYsg9HzoRbHx7PSazFwljbOzif/eSJIS
IRsPFE2Ao42PMhETdmlO0QKBgEQ6QrTNK3mZLY+Mk5JTvgwTwqJ5/swgEGXREfO8
tOPaEsEV3Ecl10K7BqEoJV0BpN1eohCZwRtbPceOeSIvfDZfQur0bMCQdt6jCOm0
6swkL5DVX50kZyA3liBV8PaD9jHHTUk9nvJ/sU/bGQl8lV/5J7Mp6qcj/646apnN
oOjJAoGAcjjSqb1rkisSK38DVpuQit53zYv4Vhchy9km21rvfSWyggEGA28ASk8W
PGScKe/wfWV5HeI7wlxMQSOz6wwwJVNWSxw3/FCbYVkS6+VRBvKHxWneWQvKqaI1
4D9VO8x2QEyZM1Vm594sgGFscPqOumdc7CFMkfBGnmXLJDy91PU=
-----END RSA PRIVATE KEY-----
)EOF";

// Function to connect to WiFi
void connectWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nWiFi Connected!");
}

// Function to connect to AWS IoT Core
void connectAWS() {
    Serial.println("Connecting to AWS IoT...");
    
    // Load AWS IoT certificates
    net.setCACert(ROOT_CA);
    net.setCertificate(CLIENT_CERT);
    net.setPrivateKey(PRIVATE_KEY);

    // Set MQTT server
    client.setServer(MQTT_HOST, 8883);
    client.setCallback(messageReceived);

    // Attempt to connect
    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nConnected to AWS IoT!");
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
}

// Function to publish temperature data
void publishTemperature() {
    int adcValue = analogRead(LM35_PIN);
    float voltage = adcValue * (5.0 / 4095.0);  // Convert ADC value to voltage
    float temperature = voltage * 100.0;  // LM35 conversion

    char payload[50];
    snprintf(payload, sizeof(payload), "{\"temperature\": %.2f}", temperature);
    
    Serial.println("Publishing: " + String(payload));
    client.publish(AWS_IOT_PUBLISH_TOPIC, payload);
}

// Callback function for receiving MQTT messages
void messageReceived(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message from topic: ");
    Serial.println(topic);
}

// Setup function
void setup() {
    Serial.begin(115200);
    analogReadResolution(12);  // Set ADC resolution to 12-bit
    connectWiFi();
    connectAWS();
}

// Loop function
void loop() {
    if (!client.connected()) {
        connectAWS();
    }
    client.loop();

    if (millis() - lastMillis > interval) {
        lastMillis = millis();
        publishTemperature();
    }
}
