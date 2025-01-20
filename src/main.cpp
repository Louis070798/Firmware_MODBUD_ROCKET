#include <SPI.h>
#include <Ethernet.h>
#include <ModbusEthernet.h>
#include <Arduino.h>

// Cấu hình Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; // Địa chỉ MAC của controller
IPAddress ip(192, 168, 1, 101);                     // Địa chỉ IP của controller
IPAddress remote(192, 168, 1, 100);                // Địa chỉ của Modbus Slave

// Cấu hình Modbus
const uint16_t REG =0;       

// Cấu hình Input và Output
const int INPUT_PIN[] = {27, A8, A9, A10, A11, A12, A13, A14, A15, A3, A2, A1, A0, A4, A5, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28};
const int NUM_INPUT_PINS = sizeof(INPUT_PIN) / sizeof(INPUT_PIN[0]);
int statePre[NUM_INPUT_PINS];
ModbusEthernet mb; 

void setup()
{
  Serial.begin(9600);     
  Ethernet.init(53);         // SS pin
  Ethernet.begin(mac, ip);  // start the Ethernet connection
  if(Ethernet.localIP())
  {Serial.println(Ethernet.localIP());}
  delay(500);              
  mb.client();              
  mb.connect(remote,502);
  mb.addCoil(0,0,15);
  for (int i = 0; i < 15; i++)
  {
    pinMode(INPUT_PIN[i], INPUT_PULLUP); 
    Serial.println("INPUT ");
    Serial.print(String(i)+ "  ");
  }
}

void loop()
{
  if (mb.isConnected(remote))
  {
    for (int i = 0; i < NUM_INPUT_PINS; i++)
    {
      bool state = digitalRead(INPUT_PIN[i]);
      if(state != statePre[i])
      {
       mb.writeCoil(remote,i,state);
       Serial.println("State "+String(i)+":"+String(state));
       statePre[i]=  state;
      }

    }
  }
  else
  {
    mb.connect(remote); // Kết nối lại nếu mất kết nối
    Serial.println("reconnect");
  }
  mb.task(); // Thực hiện các tác vụ Modbus
}
