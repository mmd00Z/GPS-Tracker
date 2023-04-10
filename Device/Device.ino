#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// import my SSID and Password
#include "WiFi_config.h" 

// Replace with your server details
const char* serverName = "https://mmd00z-organic-guide-wj6g9676j962rj9-5000.preview.app.github.dev";
const int serverPort = 5000;
const char* endpoint = "/save-coordinates";

static const int RXPin = D2, TXPin = D1;
static const uint32_t GPSBaud = 9600;

// The serial connection to the GPS device
SoftwareSerial serial2(RXPin, TXPin);

// Create TinyGPS++ object
TinyGPSPlus gps;

// Set timer interval
unsigned long timerInterval = 1000;

void setup() {
  Serial.begin(9600);
  serial2.begin(GPSBaud);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Start GPS serial communication
  serial2.begin(9600);
}

void loop() {
  // Wait for GPS data
  while (serial2.available() > 0) {
    gps.encode(serial2.read());
  }

  // Check if GPS data is valid
  if (gps.location.isValid()) {
    // Create HTTP client object
    WiFiClient client;
    HTTPClient http;

    // Build URL for server
    String url = serverName;
    url += endpoint;

    // Create JSON payload with coordinates
    String payload = "{\"latitude\": " + String(gps.location.lat(), 6) + ", \"longitude\": " + String(gps.location.lng(), 6) + "}";

    // Send POST request to server
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      Serial.println("Coordinates sent successfully");
    } else {
      Serial.print("Error sending coordinates: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  // Wait for timer to expire
  unsigned long currentTime = millis();
  static unsigned long previousTime = 0;
  if (currentTime - previousTime >= timerInterval) {
    previousTime = currentTime;
  }
}
