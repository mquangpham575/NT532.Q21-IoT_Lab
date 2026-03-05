int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int pinCount = 12;
int potPin = A0;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < pinCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Tắt đèn lúc mới bật
  }
}

void loop() {
  int potValue = analogRead(potPin);
  
  // Chuyển dải 0-1023 sang 0-12
  int ledsToLight = map(potValue, 0, 1023, 0, pinCount);

  for (int i = 0; i < pinCount; i++) {
    // Bật đèn nếu chỉ số i nhỏ hơn số lượng cần sáng
    digitalWrite(ledPins[i], (i < ledsToLight) ? HIGH : LOW);
  }
  
  delay(50);
}