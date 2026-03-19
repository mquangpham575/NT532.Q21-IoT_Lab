#define trigPin 13 // Chân phát sóng âm
#define echoPin 12 // Chân nhận sóng âm phản hồi

void setup()
{
  Serial.begin(9600); // Khởi tạo cổng Serial
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()
{
  long duration, distance;
  
  // Chu kỳ phát xung sạch
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Đo thời gian phản hồi (micro giây)
  duration = pulseIn(echoPin, HIGH);
  
  // Công thức tính cm: (thời gian / 2) / 29.1
  distance = (duration / 2) / 29.1;

  // Kiểm tra giới hạn đo của cảm biến
  if (distance >= 200 || distance <= 0)
  {
    Serial.println("Out of range"); // Ngoài phạm vi (quá xa hoặc quá gần)
  }
  else
  {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  delay(500); // Nghỉ 0.5 giây trước khi đo lần tiếp theo
}