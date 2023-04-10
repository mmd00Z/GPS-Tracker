#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with your server details
const char* serverName = "http://your_server.com";
const int serverPort = 80;
const char* endpoint = "/coordinates";

// Create TinyGPS++ object
TinyGPSPlus gps;

// Set timer interval
unsigned long timerInterval = 1000;

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Start GPS serial communication
  Serial2.begin(9600);
}

void loop() {
  // Wait for GPS data
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
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
