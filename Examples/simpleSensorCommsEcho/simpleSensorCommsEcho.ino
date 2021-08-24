/**
 * @file simpleSensorCommsEcho.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Demonstrates communicating locally with a sensor, echoing communication from the hardware serial port to the USB serial and vice/versa. Based on SerialPassthrough sketch.
 * @version 0.1
 * @date 2021-06
 * 
 * @copyright Copyright (c) 2021
 * 
 * 
 * Echoes bytes from the serial console to the hardware serial port and
 * vice-versa.
 *
 * Some boards, like the Arduino 101, the MKR1000, Zero, or the Micro, have one
 * hardware serial port attached to Digital pins 0-1, and a separate USB serial
 * port attached to the IDE Serial Monitor. This means that the "serial
 * passthrough" which is possible with the Arduino UNO (commonly used to interact
 * with devices/shields that require configuration via serial AT commands) will
 * not work by default.
 *
 * This sketch allows you to emulate the serial passthrough behaviour. Any text
 * you type in the IDE Serial monitor will be written out to the serial port on
 * Digital pins 0 and 1, and vice-versa.
 *
 * On the 101, MKR1000, Zero, and Micro, "Serial" refers to the USB Serial port
 * attached to the Serial Monitor, and "Serial1" refers to the hardware serial
 * port attached to pins 0 and 1. This sketch will emulate Serial passthrough
 * using those two Serial ports on the boards mentioned above, but you can change
 * these names to connect any two serial ports on a board that has multiple ports.
 *
 * created 23 May 2016
 * by Erik Nyquist
 *
 * modified 20 May 2021
 * by Sophie Bernier
 */
#include "wiring_private.h" // Required for pinPeripheral function.

#define PIN_SERIAL2_RX       (11ul)               // Pin description number for PIO_SERCOM on D11.
#define PIN_SERIAL2_TX       (10ul)               // Pin description number for PIO_SERCOM on D10.
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // Pin 10 is SERCOM pad 2 of this channel.
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_0)    // Pin 11 is SERCOM pad 0 of this channel.
// Declare SERCOM object with pins belonging to the desired SERCOM's primary or alternate channel and the pads corresponding to those pins. 
// TX can use the pin corresponding to UART_TX_PAD_0 or UART_TX_PAD_2
// RX can use the pin corresponding to SERCOM_RX_PAD_0, SERCOM_RX_PAD_1, SERCOM_RX_PAD_3, or SERCOM_RX_PAD_4
Uart Serial2(&sercom1,
             PIN_SERIAL2_RX,
             PIN_SERIAL2_TX,
             PAD_SERIAL2_RX,
             PAD_SERIAL2_TX);

void SERCOM1_Handler() // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}

uint8_t started = 0;
uint32_t currentMillis = 0;

void setup() {
  Serial.begin(19200);
  while (!Serial);
  Serial2.begin(4800);
  while (!Serial2);
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
}

void loop() {
  if (started == 0)
  {
    Serial.print("DBG: Sending 'ESC' 1");
    //Serial1.write(27);
    Serial2.print("GetCD"); 
    Serial2.print('\r');
    currentMillis = millis();
    started = 1;
  }
  else if ((started == 1) && (millis() > currentMillis + 1000))
  {
    Serial.print("DBG: Sending 'ESC' 2");
    //Serial1.write(27);
    Serial2.print("GetCD");
    Serial2.print('\r');
    started = 2;
  }
  if (Serial.available()) 
  {      // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial2.available()) 
  {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial2.read());   // read it and send it out Serial (USB)
  }
}
