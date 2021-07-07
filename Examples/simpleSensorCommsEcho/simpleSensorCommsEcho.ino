/*
  Based on SerialPassthrough sketch.
  
  Echoes bytes from the serial console to the hardware serial port and
  vice-versa.

  Some boards, like the Arduino 101, the MKR1000, Zero, or the Micro, have one
  hardware serial port attached to Digital pins 0-1, and a separate USB serial
  port attached to the IDE Serial Monitor. This means that the "serial
  passthrough" which is possible with the Arduino UNO (commonly used to interact
  with devices/shields that require configuration via serial AT commands) will
  not work by default.

  This sketch allows you to emulate the serial passthrough behaviour. Any text
  you type in the IDE Serial monitor will be written out to the serial port on
  Digital pins 0 and 1, and vice-versa.

  On the 101, MKR1000, Zero, and Micro, "Serial" refers to the USB Serial port
  attached to the Serial Monitor, and "Serial1" refers to the hardware serial
  port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  created 23 May 2016
  by Erik Nyquist

  modified 20 May 2021
  by Sophie Bernier
*/

#include <proO.h>

// Note. Times out after 60s of inactivity.

uint8_t started = 0;
uint32_t currentMillis = 0;
uint32_t lastTXMillis = 0;
uint32_t lastRXMillis = 0;
uint32_t firstStartMillis = 0;
uint32_t currentRXMillis = 0;
String rxBuffer;
ProCVData Data;
nextCommand_t nextCommand = nextCommand_none;
expecting_t expecting = expecting_undefined;
bool viewingLoggedData;

void setup() {
  Serial.begin(19200);
  while (!Serial);
  Serial1.begin(19200);
  while (!Serial1);
}

void loop() {
  currentMillis = millis();
  if (started == 0)
  {
    Serial.println("DBG: Sending 'ESC' 1");
    Serial1.write(27);
    lastTXMillis = millis();
    expecting = expecting_undefined;
    started = 1;
  }
  else if ((started == 1) && (currentMillis - lastTXMillis > 1000))
  {
    Serial.println("DBG: Sending 'ESC' 2");
    Serial1.write(27);
    lastTXMillis = millis();
    expecting = expecting_undefined;
    firstStartMillis = lastTXMillis;
    started = 2;
  }
  else if ((started == 2) && (currentMillis - lastTXMillis > 1000))
  {
    switch (nextCommand)
    {
      case nextCommand_startViewingLoggedData:
        Serial1.print("4\r");
        lastTXMillis = millis();
        expecting = expecting_loggedData;
        nextCommand = nextCommand_none;
        break;
      default:
        break;
    }
  }
  if ((currentMillis - lastTXMillis > 60000) && (viewingLoggedData == false))
  {
    started = 0; // restart before timeout.
  }

  if ((currentMillis - firstStartMillis > 10000) && (viewingLoggedData == false))
  {
    nextCommand = nextCommand_startViewingLoggedData;
    viewingLoggedData = true;
  }

  if ((currentMillis - lastTXMillis > 20000) && (viewingLoggedData == true))
  {
    started = 1; // exit viewing logged data.
    viewingLoggedData = false;
  }
  
  if (Serial.available()) 
  {      // If anything comes in Serial (USB),
    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) 
  {     // If anything comes in Serial1 (pins 0 & 1)
    rxBuffer = Serial1.readStringUntil('\n');
    currentRXMillis = millis();
    Serial.println(rxBuffer);
    Serial.print("DBG: ");
    Serial.print(currentRXMillis - lastRXMillis);
    Serial.print("ms since RX, ");
    Serial.print(currentRXMillis - lastTXMillis);
    Serial.println("ms since TX");
    lastRXMillis = currentRXMillis;

    switch (expecting)
    {
      case expecting_loggedData:
        Data.setDataFromString(rxBuffer);
        // Data.printAllData(Serial);
        break;
      default:
        break;
    }
    //Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }
}
