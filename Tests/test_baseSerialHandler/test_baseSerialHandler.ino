#include <baseSerialHandler.h>

baseSerialHandler bsh (Serial);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  while (!Serial)
  {
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  bsh.serviceUsb();
}
