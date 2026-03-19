// Định nghĩa chân cảm biến
const int trigPin = 9;
const int echoPin = 10;

// Chân nối LED 7 đoạn {a, b, c, d, e, f, g}
const int segPins[] = {2, 3, 4, 5, 6, 7, 8};

// Mã hex cho LED Chung Cực Dương (0: Sáng, 1: Tắt)
byte numCA[] = {
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH); // Tắt tất cả thanh LED
  }
}

// Hàm xuất số ra LED 7 đoạn
void displayNum(int n) {
  for (int i = 0; i < 7; i++) {
    // Đọc từng bit từ mảng mã hex để điều khiển chân tương ứng
    digitalWrite(segPins[i], bitRead(numCA[n], i));
  }
}

void loop() {
  // Phát xung kích hoạt cảm biến
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Đo thời gian xung phản hồi và tính khoảng cách (cm)
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  Serial.print("Distance: "); Serial.println(distance);

  // Phân mức hiển thị: Càng gần số càng lớn
  if (distance <= 0 || distance > 180) {
    displayNum(0); // Lỗi hoặc quá xa -> Hiện 0
  } else if (distance <= 20) {
    displayNum(9); 
  } else if (distance <= 40) {
    displayNum(8);
  } else if (distance <= 60) {
    displayNum(7);
  } else if (distance <= 80) {
    displayNum(6);
  } else if (distance <= 100) {
    displayNum(5);
  } else if (distance <= 120) {
    displayNum(4);
  } else if (distance <= 140) {
    displayNum(3);
  } else if (distance <= 160) {
    displayNum(2);
  } else {
    displayNum(1); 
  }

  delay(200); // Giảm tốc độ cập nhật để tránh nhấp nháy
}