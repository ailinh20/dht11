#include "dht11.h"

//Chờ DHT11 thu thập data
void waitDHT11(){
  delay(2000);
}

//Ước lượng thời gian của tín hiệu mức HIGH/LOW
unsigned long timeOfSignal(int DHT11_PIN, bool state){
  unsigned long startTime = micros();
  unsigned long _time = 0;

  while(digitalRead(DHT11_PIN) == state) _time = micros() - startTime;
  return _time;  
}

//Gửi tín hiệu Start đến DHT11
void start_Signal(int DHT11_PIN){
  pinMode(DHT11_PIN, OUTPUT); //set pin DHT11 là output để chuẩn bị gửi tín hiệu Start
  digitalWrite(DHT11_PIN, LOW);  // gửi tín hiệu LOW đến DHT11 để báo hiệu muốn bắt đầu đo nhiệt độ, độ ẩm
  delay(20); // delay tối thiểu 18ms chờ DHT11 nhận tín hiệu => chọn 20ms để an toàn.
  digitalWrite(DHT11_PIN, HIGH); //gửi tín hiệu HIGH đến DHT11 hoàn tất gửi tín hiệu Start
  pinMode(DHT11_PIN, INPUT); // set pin DHT11 là input để chuẩn bị nhận phản hồi từ cảm biến
}

bool get_Responses(int DHT11_PIN){
  unsigned long tmp_Time; 
  
  tmp_Time = timeOfSignal(DHT11_PIN, LOW); //tính thời gian ở mỨc tín hiệu LOW

 // theo datasheet thì giữ mức LOW trong khoảng 80us, nhưng khi chạy thực tế
 // đối với lần đọc đầu tiên dao động khoảng 66-67us, những lần đo sau dao
 // động khoảng 76-77us => chọn 65 để an toàn 
  if (tmp_Time < 65) {
    Serial.print(tmp_Time);
    Serial.println("\tERROR_TIMEOUT1");
    return false;
  }

    tmp_Time = timeOfSignal(DHT11_PIN, HIGH); //tính thời gian ở mức tín hiệu HIGH
  if (tmp_Time < 80) {
    Serial.print(tmp_Time);
    Serial.println("\tERROR_TIMEOUT2");
    return false;
  }
  return true;    
}

void readData_DHT11(int DHT11_PIN){
  uint8_t data[5];
  unsigned long signals[80]; // 40 bits data, 1 bit data cần 2 tín hiệu => 40*2 = 80
  for(int i = 0; i < 80; i += 2) {
    signals[i] = timeOfSignal(DHT11_PIN, LOW); // lưu thời gian ở mức LOW của bit data thứ i/2
    signals[i+1] = timeOfSignal(DHT11_PIN, HIGH); // lưu thời gian ở mức HIGH của bit data thứ i/2
  }
  for (int i = 0; i < 5; i++){ //lưu 5 byte data ~ 40 bits data
    for (int j=0; j < 16; j += 2)  // chạy 8 lần tương ứng với 8 bits data 
    {
    // kiểm tra bit data = "0", theo datasheet khi mức LOW giữ khoảng 50us sau đó mức HIGH giữ
    // khoảng 26-28us, nhưng khi chạy thực tế mức LOW giữ khoảng 52-53us => chọn 55us
      if (signals[i*16 + j] < 55 && signals[i*16 + j+1] < 28){   
        data[i] <<= 1;
      } else 
      // kiểm tra bit data = "1", theo datasheet khi mức LOW giữ khoảng 50us sau đó mức HIGH
      // giữ khoảng 70us, nhưng khi chạy thực tế mức LOW giữ khoảng 52-53us => chọn 55us
      // còn HIGH giữ khoảng 70-71us => chọn 72us để an toàn       
      if (signals[i*16 + j] < 55 && signals[i*16 + j+1] < 72){
        data[i] <<= 1;
        data[i] |= 1;
      } else {      
      }
    }
  }
  if (data[4] == data[0]+data[1]+data[2]+data[3]){ // kiểm tra tổng
    Serial.print("Humi: ");
    Serial.print(data[0]);
    Serial.print(".");
    Serial.print(data[1]);
    Serial.print("% Temp: ");
    Serial.print(data[2]);
    Serial.print(".");
    Serial.print(data[3]);
    Serial.println(" degrees"); 
  } else Serial.println("ERROR_CHECKSUM"); 
}