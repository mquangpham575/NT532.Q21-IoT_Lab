const int ledPins[] = {2, 3, 4, 5, 6, 7}; 
const int buttonPin = 8; 

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Tắt đèn lúc mới bật
  }
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    delay(50); 
    if (digitalRead(buttonPin) == LOW) {
      // Tạo số random
      int randomNumber = random(1, 7);

      // In ra kết quả random
      Serial.println(randomNumber);

      // Hiệu ứng chạy đèn
      for (int i = 0; i < 6; i++) {
        digitalWrite(ledPins[i], HIGH);
        delay(80);
        digitalWrite(ledPins[i], LOW);
      }

      // Hiển thị số đèn quay được
      for (int i = 0; i < 6; i++) {
        digitalWrite(ledPins[i], (i < randomNumber) ? HIGH : LOW);
      }
      delay(1000); 

      // Chớp tắt báo hiệu
      for (int k = 0; k < 6; k++) {
        for (int j = 0; j < 6; j++) digitalWrite(ledPins[j], HIGH);
        delay(100);
        for (int j = 0; j < 6; j++) digitalWrite(ledPins[j], LOW);
        delay(100);
      }

      // Chờ nhả nút
      while (digitalRead(buttonPin) == LOW) delay(10); 
    }
  }
}