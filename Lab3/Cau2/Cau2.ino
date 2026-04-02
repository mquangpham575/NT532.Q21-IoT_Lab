#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>

const char* ssid = "test123";
const char* password = "12345678";
const char* serverUrl = "http://192.168.79.16:5000/api/sensors"; 

// Khai báo cảm biến ánh sáng kỹ thuật số
BH1750 lightMeter;

// Cập nhật chân cảm biến siêu âm
const int trigPin = 0;  // D3
const int echoPin = 2;  // D4
const int ledPins[] = {14, 12, 13}; // D5, D6, D7

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo I2C và BH1750
  Wire.begin(); 
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Initialized"));
  } else {
    Serial.println(F("Error initializing BH1750"));
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for(int i=0; i<3; i++) pinMode(ledPins[i], OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Connected: " + WiFi.localIP().toString());
}

long getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Đọc giá trị Lux từ GY-302
    float lux = lightMeter.readLightLevel();
    long distance = getDistance();

    Serial.printf("Lux: %.2f | Dist: %ld cm\n", lux, distance);

    // Tạo JSON
    StaticJsonDocument<200> doc;
    doc["error"] = false;
    doc["message"] = "Data from Wemos";
    JsonObject dataObj = doc.createNestedObject("data");
    dataObj["light"] = lux;      // Gửi giá trị Lux
    dataObj["distance"] = distance;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
      String response = http.getString();
      StaticJsonDocument<200> resDoc;
      deserializeJson(resDoc, response);
      int ledCount = resDoc["data"]["led_count"];
      
      // Điều khiển LED dựa trên phản hồi từ Server
      for(int i=0; i<3; i++) digitalWrite(ledPins[i], (i < ledCount) ? HIGH : LOW);
    }
    http.end();
  }
  delay(5000); 
}