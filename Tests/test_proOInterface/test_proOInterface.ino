#include <proO.h>

ProCVCallbacks_t callbacks = {};

uint8_t started = 0; 
uint32_t currentMillis = 0;
ProCV CO2Sensor(Serial1, callbacks);

void setup()
{
  Serial.begin(19200);
  while (!Serial)
  {
    delay(1); 
  }
  Serial.println("Serial started.");
  Serial1.begin(19200);
  while (!Serial1)
  {
    delay(1);
  }
  Serial.println("Serial1 started.");
  CO2Sensor.startViewingLoggedData();
}

void loop()
{
  if (Serial.available()) 
  {      // If anything comes in Serial (USB),
    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }
  CO2Sensor.serviceSerial();
}
