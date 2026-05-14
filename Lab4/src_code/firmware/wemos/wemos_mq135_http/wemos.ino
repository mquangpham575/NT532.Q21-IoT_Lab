#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// WiFi Configuration
const char* ssid = "iot-nhom9";
const char* password = "12312312";

// Custom Backend Configuration
// NOTE: Change host to your computer's IP address (e.g., 192.168.1.10)
const char* host = "192.168.1.100"; 
const char* token = "wemos_d1";
const int port = 8000;

// Hardware Configuration
const int mq135Pin = A0;

// Connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Read MQ-135 value
    int airQuality = analogRead(mq135Pin);
    
    // Prepare JSON payload
    String payload = "{\"air_quality\":" + String(airQuality) + "}";
    
    // Construct URL
    String url = "http://" + String(host) + ":" + String(port) + "/api/v1/" + String(token) + "/telemetry";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response code: " + String(httpResponseCode));
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    
    http.end();
  }
  
  // Wait 5 seconds before next reading
  delay(5000);
}
