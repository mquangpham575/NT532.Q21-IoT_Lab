#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "test123";
const char* password = "12345678";

const int ledPins[] = {14, 12, 13}; 
int score = 0;
int targetLED = 0;
String statusMsg = "Bấm BẮT ĐẦU để chơi!";

ESP8266WebServer server(80);

void startupAnimation() {
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 3; i++) { digitalWrite(ledPins[i], HIGH); delay(100); digitalWrite(ledPins[i], LOW); }
    for (int i = 1; i >= 0; i--) { digitalWrite(ledPins[i], HIGH); delay(100); digitalWrite(ledPins[i], LOW); }
  }
}

void handleRoot() {
  String h = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  h += "<style>body{font-family:sans-serif;text-align:center;padding-top:20px;background:#f0f2f5;} .card{background:white;margin:auto;width:90%;max-width:400px;padding:20px;border-radius:15px;box-shadow:0 4px 6px rgba(0,0,0,0.1);}";
  h += ".btn{padding:15px;width:80px;font-size:18px;margin:5px;cursor:pointer;border:none;border-radius:8px;color:white;font-weight:bold;} .b1{background:#ff4757;} .b2{background:#ffa502;} .b3{background:#2ed573;}";
  h += ".score{font-size:40px;font-weight:bold;color:#1e90ff;margin:10px 0;} .start{background:#2f3542;color:white;padding:15px 30px;text-decoration:none;display:inline-block;margin-top:20px;border-radius:50px;}</style></head><body><div class='card'>";
  
  h += "<h1>HÃY CHỌN ĐÈN ĐÚNG</h1><div class='score'>" + String(score) + "</div><p>ĐIỂM SỐ</p><h2>" + statusMsg + "</h2>";
  h += "<div><a href='/?choice=1'><button class='btn b1'>1</button></a><a href='/?choice=2'><button class='btn b2'>2</button></a><a href='/?choice=3'><button class='btn b3'>3</button></a></div>";
  h += "<br><a href='/newround' class='start'>BẮT ĐẦU LƯỢT MỚI</a>";
  
  if(targetLED != 0) h += "<p style='color:#ccc;font-size:10px;margin-top:20px;'>Xác nhận: Đèn đúng là " + String(targetLED) + "</p>";
  h += "</div></body></html>";
  
  server.send(200, "text/html", h);
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) pinMode(ledPins[i], OUTPUT);
  startupAnimation();
  
  randomSeed(analogRead(A0) + micros()); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nĐÃ KẾT NỐI!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    if (server.hasArg("choice")) {
      int choice = server.arg("choice").toInt();
      if (targetLED != 0) {
        if (choice == targetLED) { 
            score += 10; 
            statusMsg = "<span style='color:green;'>CHÍNH XÁC! Bạn được +10đ</span>"; 
        }
        else { 
            score -= 5; 
            statusMsg = "<span style='color:red;'>SAI RỒI! Đáp án là số " + String(targetLED) + "</span>"; 
        }
        targetLED = 0;
      }
    }
    handleRoot();
  });

  server.on("/newround", []() {
    targetLED = random(1, 4);
    Serial.print(">>> Đáp án lượt này: "); Serial.println(targetLED);
    statusMsg = "Đã giấu đèn bí mật! Đoán đi?";
    handleRoot();
    digitalWrite(ledPins[targetLED-1], HIGH); delay(500); digitalWrite(ledPins[targetLED-1], LOW);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}