#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "test123"; 
const char* password = "12345678";

#define LED_PIN LED_BUILTIN
ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>Server is Running!</h1><a href='/ledOn'>ON</a> | <a href='/ledOff'>OFF</a>");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);

  WiFi.softAPdisconnect(true); 
  WiFi.disconnect();           
  delay(1000);
  WiFi.mode(WIFI_STA);         
  
  Serial.println("\nDang tim WiFi: test123...");
  WiFi.begin(ssid, password);

  // Đợi kết nối
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter > 30) { 
      Serial.println("\nLau qua, dang thu ket noi lai...");
      WiFi.begin(ssid, password);
      counter = 0;
    }
  }

  Serial.println("\nDA KET NOI THANH CONG!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/ledOn", [](){
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", "LED ON. <a href='/'>Back</a>");
  });
  server.on("/ledOff", [](){
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/html", "LED OFF. <a href='/'>Back</a>");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}