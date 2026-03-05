const int ledPins[] = {2, 3, 4};
const int potPin = A0;
const int pinCount = 3;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < pinCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Tắt đèn lúc mới bật
  }
}

void loop() {
  int potValue = analogRead(potPin);
  int waitTime;
  String speedStatus;

  // Phân loại tốc độ dựa trên giá trị biến trở
  if (potValue < 341) { 
    waitTime = 1000; 
    speedStatus = "Cham";
  } 
  else if (potValue < 682) { 
    waitTime = 400; 
    speedStatus = "Trung binh";
  } 
  else { 
    waitTime = 100; 
    speedStatus = "Nhanh";
  }

  // In thông tin ra Terminal
  Serial.print("Toc do: ");
  Serial.println(speedStatus);

  // Hiệu ứng chạy đèn lần lượt
  for (int i = 0; i < pinCount; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(waitTime);
    digitalWrite(ledPins[i], LOW);
  }
}