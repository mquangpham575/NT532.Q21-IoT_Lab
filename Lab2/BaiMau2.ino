int sensorPin = 7;     // Chân tín hiệu cảm biến
int sensorState = 0;   // Biến lưu trạng thái hiện tại

void setup() {
  Serial.begin(9600);       
  pinMode(sensorPin, INPUT);     // Thiết lập chân cảm biến làm đầu vào
  pinMode(LED_BUILTIN, OUTPUT);  // LED có sẵn trên mạch (chân 13) làm đầu ra
}

void loop() {
  // Đọc tín hiệu số (0 hoặc 1)
  sensorState = digitalRead(sensorPin); 
  
  Serial.print("Tin hieu: ");
  Serial.println(sensorState); 

  // Nếu có tín hiệu (mức CAO)
  if (sensorState == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH); // Bật LED trên mạch
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // Tắt LED
  }
  
  delay(200); // Khoảng nghỉ để dữ liệu Serial không bị trôi quá nhanh
}