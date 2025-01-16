#include <SPI.h>
#include <Ethernet.h>      
#include <ModbusEthernet.h>
#include <Arduino.h>

// Cấu hình Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE }; // Địa chỉ MAC của controller
IPAddress ip(192, 168, 1, 54); // Địa chỉ IP của controller
IPAddress remote(192, 168, 1, 100);  // Địa chỉ của Modbus Slave

// Cấu hình Modbus
const uint16_t REG = 512;               // Modbus Hreg Offset
const int32_t showDelay =100;         // Thời gian hiển thị kết quả mỗi 5 giây

// Mảng các chân input (A0-A15)
const int INPUT_PIN[] = {A8, A9, A10, A11, A12, A13, A14, A15, A3, A2, A1, A0, A4, A5}; // , A6, A7};
const int OUTPUT_PIN[]={37,36,35,34,33,32,31,30,29,28}; // ,27,26,25,24,23,22};
const int NUM_INPUT_PINS = sizeof(INPUT_PIN) / sizeof(INPUT_PIN[0]);

ModbusEthernet mb;               // Khởi tạo ModbusTCP instance
void statemachine();
void setup() {
  Serial.begin(9600);     // Mở kết nối Serial và chờ mở cổng
  #if defined(AVR_LEONARDO)
  while (!Serial) {}        // Đợi kết nối Serial, chỉ cần thiết cho Leonardo
  #endif
  
  Ethernet.init(53);         // Cấu hình chân SS
  Ethernet.begin(mac, ip);  // Khởi tạo kết nối Ethernet
  delay(1000);              // Cho Ethernet shield một giây để khởi động
  mb.client();              // Thiết lập Modbus TCP server
  Serial.println(Ethernet.localIP());
  
  // Cấu hình các chân IO làm đầu vào với Pull-Up
  for (int i = 0; i < NUM_INPUT_PINS; i++) {
    pinMode(INPUT_PIN[i], INPUT_PULLUP); // Cấu hình các chân làm đầu vào
  }
  for (int i = 0; i < 10; i++) {
    pinMode(OUTPUT_PIN[i], OUTPUT); // Cấu hình các chân làm đầu vào
  }

}

uint16_t res = 0;
uint32_t showLast = 0;

void loop() {
  if (mb.isConnected(remote)) {   // Kiểm tra nếu kết nối với Modbus Slave thành công
    // Đọc giá trị các chân input và cập nhật vào các thanh ghi Modbus
    for (int i = 0; i < NUM_INPUT_PINS; i++) {
      bool state = digitalRead(INPUT_PIN[i]) == LOW;  // Đọc trạng thái chân, LOW nếu nhấn nút
      uint16_t value = state ? 1 : 0;  // Chuyển giá trị boolean thành 1 (ON) hoặc 0 (OFF)
      mb.writeHreg(remote, REG + i, value);  // Ghi giá trị vào các thanh ghi Modbus tương ứng
    }
  } else {
    mb.connect(remote);           // Thử kết nối lại nếu chưa kết nối
  }
  delay(100);                     // Khoảng thời gian giữa các lần lặp
  mb.task();                      // Thực hiện các tác vụ Modbus cục bộ

  // Hiển thị giá trị mỗi 5 giây
  if (millis() - showLast > showDelay) {
    showLast = millis();
    // Hiển thị trạng thái của các chân
    for (int i = 0; i < NUM_INPUT_PINS; i++) {
      bool state = digitalRead(INPUT_PIN[i]) == LOW;  // Đọc trạng thái chân
      // Serial.print("Pin A");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(state ? "LOW (Pressed)" : "HIGH (Released)");
    }
    statemachine();
  }
}
void statemachine()
{
 // Chế đọ Bám, bắt mục tiêu của đầu tự dẫn 24H1
  if(digitalRead(INPUT_PIN[6]) == 1)
  {
    Serial.println("ТАКТИЧ  ПУСК" + String(digitalRead(INPUT_PIN[6])));
    if(digitalRead(INPUT_PIN[0]) == 1 )
    {
      Serial.println("ПИТАН. АВ " + String(digitalRead(INPUT_PIN[0])));
    }
  }
}