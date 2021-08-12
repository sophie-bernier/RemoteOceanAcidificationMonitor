/**
 * @file Serial_1_2_TX.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Demonstrates transmitting messages on the default hardware serial (Serial1) and on a newly instantiated hardware serial on pins 10/11 (Serial2).
 * @version 0.1
 * @date 2021-08-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "wiring_private.h"
#define PIN_SERIAL2_RX       (11ul)               // Pin description number for PIO_SERCOM on D11.
#define PIN_SERIAL2_TX       (10ul)               // Pin description number for PIO_SERCOM on D10.
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // Pin 11 is SERCOM pad 0 of this channel.
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_0)    // Pin 11 is SERCOM pad 2 of this channel.
Uart Serial2(&sercom1,
              PIN_SERIAL2_RX,
              PIN_SERIAL2_TX,
              PAD_SERIAL2_RX,
              PAD_SERIAL2_TX);

uint8_t myByte = 0;

void setup() {
  // Blink on startup.
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13, 0);
  delay(500);
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13, 0);
  delay(500);
  Serial.begin(9600); // Initialize serial.
  while(!Serial); // Wait until serial is initialized.
  Serial1.begin(9600);
  while(!Serial1);
  Serial2.begin(9600);
  while(!Serial2);
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13, 0);
  delay(500);
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13, 0); 
  delay(500);

  // Configure Arduino Pins 10 and 11 to serve their primary SERCOM.
  // Some Pins have an alternate SERCOM.
  // In that case we would replace PIO_SERCOM with PIO_SERCOM_ALT.
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
}

void loop() {
  // Blink LED
  static bool led = 1;
  if (led == 1)
  {
    led = 0;
    digitalWrite(13, 1);
  }
  else
  {
    led = 1;
    digitalWrite(13, 0);
  }

  // Print to Serial, Serial1, and Serial2
  Serial.println("hello");
  Serial1.println("hellOwOrld");
  Serial2.println("hellUwUrld");

  // Echo Serial inputs to Serial.
  if (Serial.available() > 0)
  {
    myByte = Serial.read();
    Serial.println(myByte);
  }
  
  delay(1000);
}

void SERCOM1_Handler()    // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
