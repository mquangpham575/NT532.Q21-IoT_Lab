const int triggerPin = 9;
const int echoPin = 10;
const int ledPin = 13;

long duration;
int distance;

void setup() {
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Phát xung siêu âm
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Đo thời gian phản hồi (micro giây)
  duration = pulseIn(echoPin, HIGH);

  // Quy đổi sang cm (vận tốc âm thanh / 2)
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Phân cấp tốc độ chớp tắt theo khoảng cách
  if (distance >= 0 && distance < 20) {
    blink(100); // Rất gần - chớp cực nhanh
  } else if (distance >= 20 && distance < 40) {
    blink(300);
  } else if (distance >= 40 && distance < 60) {
    blink(600);
  } else if (distance >= 60 && distance < 80) {
    blink(900);
  } else if (distance >= 80) {
    blink(1200); // Xa - chớp rất chậm
  }
}

// Hàm điều khiển bật/tắt đèn LED
void blink(int delayTime) {
  digitalWrite(ledPin, HIGH);
  delay(delayTime);
  digitalWrite(ledPin, LOW);
  delay(delayTime);
}