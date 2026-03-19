void setup() {
  Serial.begin(9600); // Khởi tạo cổng Serial để xem kết quả
  pinMode(pirPin, INPUT); // Thiết lập chân cảm biến là đầu vào
}

void loop() {
  // Đọc giá trị kỹ thuật số (0 hoặc 1) từ cảm biến
  int sensorValue = digitalRead(pirPin);

  if (sensorValue == HIGH) {
    // Nếu phát hiện hồng ngoại chuyển động
    Serial.println("Motion detected!");
  } else {
    // Nếu môi trường tĩnh, không có chuyển động
    Serial.println("No motion detected.");
  }

  delay(500); // Nghỉ 0.5 giây để tránh tràn dữ liệu Serial
}