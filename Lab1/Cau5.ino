const int ledPins[] = {2,3,4,5,6,7,9,10,11,12,13,A1,A2,A3,A4,A5}; // 16 chân
const int btn = 8;
int currentPos = 0, target1, target2;
int level = 1, score = 0, speed = 500;

void setup() {
  Serial.begin(115200);
  for(int i=0; i<16; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  resetGame();
}

void resetGame() {
  level = 1; score = 0; speed = 500;
  target1 = random(0, 8); target2 = random(8, 16);
  Serial.println("Level 1 - Bat dau!");
}

void blinkAll(int times) {
  for(int i=0; i<times; i++) {
    for(int j=0; j<16; j++) digitalWrite(ledPins[j], HIGH);
    delay(100);
    for(int j=0; j<16; j++) digitalWrite(ledPins[j], LOW);
    delay(100);
  }
}

void loop() {
  // Hiệu ứng chớp tắt 2 lần tại đèn hiện tại trước khi di chuyển
  for(int b=0; b<2; b++) {
    digitalWrite(ledPins[currentPos], HIGH); delay(speed/4);
    digitalWrite(ledPins[currentPos], LOW); delay(speed/4);
  }

  // Kiểm tra nút bấm
  if(digitalRead(btn) == LOW) {
    if(currentPos == target1 || currentPos == target2) {
      level++;
      score += (level * 10);
      speed = max(100, speed - 150);
      Serial.print("DUNG! Level: "); Serial.print(level);
      Serial.print(" | Diem: "); Serial.println(score);
      blinkAll(6);
      if(level > 3) { Serial.println("WINNER!"); resetGame(); }
    } else {
      score = max(0, score - 5);
      Serial.print("SAI! Diem: "); Serial.println(score);
      resetGame();
    }
    while(digitalRead(btn) == LOW); // Đợi nhả nút
  }

  currentPos = (currentPos + 1) % 16;
}