// Khai báo các chân cắm
const int pirPin = 7;    // Chân tín hiệu cảm biến PIR
const int ledPin1 = 12;  // LED báo động 1
const int ledPin2 = 13;  // LED báo động 2

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  Serial.println("He thong bao dong dang khoi dong...");
  // Chờ cảm biến ổn định nhiệt độ môi trường (10 giây)
  delay(10000); 
  Serial.println("San sang!");
}

void loop() {
  // Đọc trạng thái từ cảm biến (HIGH: có chuyển động)
  int sensorValue = digitalRead(pirPin);

  if (sensorValue == HIGH) { 
    Serial.println("Canh bao: Phat hien chuyen dong!");
    blinkLEDs(); // Chuyển sang chế độ nháy đèn báo động
  } else {
    Serial.println("An toan: Khong co chuyen dong.");
    digitalWrite(ledPin1, LOW); // Tắt đèn khi an toàn
    digitalWrite(ledPin2, LOW);
  }

  delay(500); // Khoảng nghỉ nhỏ để ổn định hệ thống
}

// Hàm chớp tắt luân phiên 2 đèn LED
void blinkLEDs() {
  // Vòng lặp chạy liên tục nếu cảm biến vẫn đang báo HIGH
  while (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin1, HIGH); // LED 1 sáng, LED 2 tắt
    digitalWrite(ledPin2, LOW);
    delay(300); 
    
    digitalWrite(ledPin1, LOW);  // LED 1 tắt, LED 2 sáng
    digitalWrite(ledPin2, HIGH);
    delay(300);
    
    Serial.println("...Dang bao dong!...");
  }
}