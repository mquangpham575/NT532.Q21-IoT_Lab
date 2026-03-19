#include "DHT.h"

#define DHTPIN 2      // Chân tín hiệu DHT
#define DHTTYPE DHT11 // Loại cảm biến (DHT11 hoặc DHT22)

#define LED_FAN1 3    // LED mô phỏng quạt mức 1
#define LED_FAN2 4    // LED mô phỏng quạt mức 2
#define LED_FAN3 5    // LED mô phỏng quạt mức 3

// Các ngưỡng nhiệt độ và độ ẩm
float TEMP_LOW = 25.0;   
float TEMP_M1 = 30.0;    
float TEMP_M2 = 35.0;    
float TEMP_M3 = 40.0;    
float HUMID_HIGH = 80.0; 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LED_FAN1, OUTPUT);
  pinMode(LED_FAN2, OUTPUT);
  pinMode(LED_FAN3, OUTPUT);
  Serial.println("He thong tan nhiet DataCenter san sang!");
}

void loop() {
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();

  // Kiểm tra lỗi đọc cảm biến
  if (isnan(temp) || isnan(humid)) {
    Serial.println("Loi doc cam bien!");
    return;
  }

  Serial.print("Nhiet do: "); Serial.print(temp);
  Serial.print(" C | Do am: "); Serial.print(humid); Serial.println(" %");

  // Logic điều khiển quạt (LED) theo nhiệt độ
  if (temp <= TEMP_LOW) {
    allFansOff(); // Nhiệt độ thấp: Tắt tất cả
  }
  else if (temp > TEMP_LOW && temp <= TEMP_M1) {
    digitalWrite(LED_FAN1, HIGH); // Mức 1: Bật 1 LED
    digitalWrite(LED_FAN2, LOW);
    digitalWrite(LED_FAN3, LOW);
  }
  else if (temp > TEMP_M1 && temp <= TEMP_M2) {
    digitalWrite(LED_FAN1, HIGH); // Mức 2: Bật 2 LED
    digitalWrite(LED_FAN2, HIGH);
    digitalWrite(LED_FAN3, LOW);
  }
  else {
    digitalWrite(LED_FAN1, HIGH); // Mức 3: Bật cả 3 LED
    digitalWrite(LED_FAN2, HIGH);
    digitalWrite(LED_FAN3, HIGH);
  }

  // Bảo vệ: Nếu độ ẩm quá cao và đã mát thì ngắt quạt hoàn toàn
  if (humid > HUMID_HIGH && temp <= TEMP_LOW) {
    Serial.println("Canh bao: Do am cao -> Ngat quat.");
    allFansOff();
  }

  delay(2000); // Chờ 2 giây giữa các lần đọc
}

// Hàm tắt toàn bộ đèn LED
void allFansOff() {
  digitalWrite(LED_FAN1, LOW);
  digitalWrite(LED_FAN2, LOW);
  digitalWrite(LED_FAN3, LOW);
}