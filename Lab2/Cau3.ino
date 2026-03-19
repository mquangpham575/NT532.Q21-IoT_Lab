#include <MQ135.h>

const int PIN_MQ135 = A0; // Chân Analog đọc dữ liệu khí
const int LED_PIN = 13;   // LED báo hiệu trạng thái
MQ135 mq135_sensor(PIN_MQ135);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Dang khoi dong cam bien MQ-135...");
}

// Hàm nháy đèn với thời gian chờ tùy chỉnh
void blynk(int duration) {
  digitalWrite(LED_PIN, HIGH);
  delay(duration);
  digitalWrite(LED_PIN, LOW);
  delay(duration);
}

void loop() {
  // Đọc giá trị điện trở và nồng độ PPM từ cảm biến
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();

  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.println(" ppm");

  // Phân cấp cảnh báo dựa trên nồng độ PPM
  if (ppm > 50 && ppm <= 150) {
    Serial.println("Chat luong khong khi: Trung binh");
    blynk(500); // Nháy vừa
  } else if (ppm > 150) {
    Serial.println("Chat luong khong khi: O nhiem!");
    blynk(100); // Nháy nhanh (Cảnh báo nguy hiểm)
  } else {
    Serial.println("Chat luong khong khi: Tot");
    blynk(1000); // Nháy chậm (An toàn)
  }

  delay(200); // Khoảng nghỉ ngắn để ổn định dữ liệu
}