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

enum state_t
{
  state_asleep,
  state_idle,
  state_menu,
  state_viewLoggedData
};

enum command_t
{
  command_none,
  command_wakeup,
  command_startViewingLoggedData,
  command_stopViewingLoggedData
};

// Note. Times out after 60s of inactivity.

uint8_t started = 0;
uint32_t currentMillis = 0;
uint32_t lastTXMillis = 0;
uint32_t lastRXMillis = 0;
uint32_t firstStartMillis = 0;
uint32_t currentRXMillis = 0;
String rxBuffer;
String txBuffer;
ProCVData Data;
command_t command = command_wakeup;
state_t state = state_asleep;
bool newRx = false;

void sendEsc ()
{
  Serial.println("DBG: Sending 'ESC'");
  Serial1.write(27);
  lastTXMillis = millis();
}

void setup()
{
  Serial.begin(19200);
  while (!Serial);
  Serial1.begin(19200);
  while (!Serial1);
}

void loop()
{
  currentMillis = millis();

  if (Serial.available()) 
  {      // If anything comes in Serial (USB),
    txBuffer = Serial.readStringUntil('\n');
    if ((txBuffer[0] = '0') && (txBuffer[1] = 'd'))
    {
      Serial.write(txBuffer.substring(2).toInt());
    }
    else
    {
      Serial1.print(txBuffer);   // read it and send it out Serial1 (pins 0 & 1)
    }
  }

  if (Serial1.available()) 
  {     // If anything comes in Serial1 (pins 0 & 1)
    rxBuffer = Serial1.readStringUntil('\n');
    currentRXMillis = millis();
    newRx = true;
    Serial.println(rxBuffer);
    Serial.print("DBG: ");
    Serial.print(currentRXMillis - lastRXMillis);
    Serial.print("ms since RX, ");
    Serial.print(currentRXMillis - lastTXMillis);
    Serial.println("ms since TX");
    lastRXMillis = currentRXMillis;
  }

  switch (state)
  {
    case state_asleep:
      switch (command)
      {
        case command_wakeup:
          sendEsc();
          state = state_idle;
          command = command_none;
      }
      break;
    case state_idle:
      if (currentMillis - lastTXMillis > 1000) // time for status to show
      {
        sendEsc();
        state = state_menu;
      }
      break;
    case state_menu:
      if (currentMillis - lastTXMillis > 1000) // time for menu to show
      {
        switch (command)
        {
          case command_startViewingLoggedData:
            Serial1.print("4\r");
            lastTXMillis = millis();
            state = state_viewLoggedData;
            command = command_none;
        }
      }
      if (currentMillis - lastTXMillis > 10000) // debug - start viewing data after a while
      {
        command = command_startViewingLoggedData;
      }
      if (currentMillis - lastTXMillis > 60000)
      {
        state = state_asleep;
        command = command_wakeup;
      }
      break;
    case state_viewLoggedData:
      if (newRx)
      {
        Data.setDataFromString(rxBuffer);
        // Data.printAllData(Serial);
      }
      if (currentMillis - lastTXMillis > 20000) // debug - stop viewing data after a while
      {
        command = command_stopViewingLoggedData;
      }
      if (currentMillis - lastRXMillis > 1000) // accept new commands as sensor returns to menu  when reading is done
      {
        state = state_menu;
      }
      switch (command)
      {
        case command_stopViewingLoggedData:
          state = state_idle; // send esc and return to main menu
      }
      break;
    default:
      break;
  }
}
