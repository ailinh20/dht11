#ifndef DHT11_H
#define DHT11_H

#include "Arduino.h"

void waitDHT11();
unsigned long timeOfSignal(int DHT11_PIN, bool state);
void start_Signal(int DHT11_PIN);
bool get_Responses(int DHT11_PIN);
void readData_DHT11(int DHT11_PIN);

#endif