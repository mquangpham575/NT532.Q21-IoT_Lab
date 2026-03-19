#include <Wire.h>
#include <BH1750.h>

BH1750 lightSensor;
const int buttonPin = 12;
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

int currentMode = 1; // 0: Chế độ 5 đèn, 1: Chế độ 10 đèn
bool firstPressed = false;
unsigned long lastPressTime = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lightSensor.begin();
  
  pinMode(buttonPin, INPUT_PULLUP); // Nút bấm nối GND
  for (int i = 0; i < 10; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  float lightValue = lightSensor.readLightLevel();

  // --- Logic quét nút bấm (1 lần -> Mode 0, 2 lần -> Mode 1) ---
  if (digitalRead(buttonPin) == LOW) {
    delay(200); // Chống dội nút
    if (!firstPressed) {
      firstPressed = true;
      lastPressTime = millis();
      currentMode = 0; // Tạm thời nhận là bấm 1 lần
      Serial.println("Che do 1: Toi da 5 den");
    } else {
      currentMode = 1; // Bấm lần 2 trong thời gian ngắn
      firstPressed = false;
      Serial.println("Che do 2: Toi da 10 den");
    }
    while(digitalRead(buttonPin) == LOW); // Đợi thả nút
  }

  // Quá 1 giây không bấm thêm thì reset trạng thái chờ
  if (firstPressed && (millis() - lastPressTime > 1000)) {
    firstPressed = false;
  }

  // --- Tính toán số lượng đèn dựa trên ánh sáng ---
  // Ánh sáng càng cao (max 1000 lux), số đèn cần bù càng ít
  int maxAllowed = (currentMode == 0) ? 5 : 10;
  int numLedsOn = map(lightValue, 0, 1000, maxAllowed, 0);
  numLedsOn = constrain(numLedsOn, 0, maxAllowed);

  Serial.print("Anh sang: "); Serial.print(lightValue);
  Serial.print(" | So den sang: "); Serial.println(numLedsOn);

  // --- Điều khiển bật/tắt đèn ---
  for (int i = 0; i < 10; i++) {
    if (i < numLedsOn) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }

  delay(200);
}