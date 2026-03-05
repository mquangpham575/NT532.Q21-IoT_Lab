const int segPins[] = {2, 3, 4, 5, 6, 7, 8};
const int trafficPins[] = {10, 11, 12}; // 10: Đỏ, 11: Vàng, 12: Xanh

const byte numbers[] = {
  0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

void setup() {
  for (int i = 2; i <= 12; i++) pinMode(i, OUTPUT);
}

void displayNum(int num) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], bitRead(numbers[num], i));
  }
}

void runTraffic(int pin, int duration) {
  for (int i = 0; i < 3; i++) digitalWrite(trafficPins[i], LOW);
  digitalWrite(pin, HIGH);

  for (int t = duration; t >= 0; t--) {
    displayNum(t);
    delay(1000);
  }
}

void loop() {
  runTraffic(12, 9); // Xanh sáng 9 giây
  runTraffic(11, 2); // Vàng sáng 2 giây
  runTraffic(10, 6); // Đỏ sáng 6 giây
}