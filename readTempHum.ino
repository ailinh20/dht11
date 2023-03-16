#include "dht11.h"

#define DHT11_PIN 32
void setup() {
  Serial.begin(9600);
}

void loop() {
  waitDHT11();
  Serial.println("----------------------"); 
  
  // gửi tín hiệu Start
  start_Signal(DHT11_PIN);
  
  unsigned long startTime = micros();
  unsigned long check;

  // chờ DHT11 kéo chân DATA xuống mức LOW để biết cảm biến có phản hồi, khoảng thời gian chờ tối đa là 40us
  while((check = (micros() - startTime)) <= 40 && digitalRead(DHT11_PIN)){} 

  if (check > 40) Serial.println("ERROR_NO_RESPONSES");
  else {
    if (get_Responses(DHT11_PIN)) readData_DHT11(DHT11_PIN);   
    else Serial.println("ERROR_NO_RESPONSES");     
  }
  
  delay(2000);
}