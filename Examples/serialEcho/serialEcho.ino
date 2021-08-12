/**
 * @file serialEcho.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief The simplest possible implementation of echoing between the USB serial port and the radio. No sensor integration.
 * @version 0.1
 * @date 2021-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 900.0
#define RH_RF95_MAX_MESSAGE_LEN 32

RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t inputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t inputBufIdx = 0;
uint8_t outputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
uint8_t inputChar = 0;

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial)
  {
    delay(1);
  }

  delay(100);

  Serial.println("Feather LoRa Echo Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop() {
  // Transmit a string!
  if (Serial.available())
  {
    inputChar = Serial.read();
    // Add chars to ignore to this.
    if ((inputChar != '\n'
         || inputChar != '\r')
        && inputBufIdx < RH_RF95_MAX_MESSAGE_LEN)
    {
      inputBuf[inputBufIdx] = inputChar;
      inputBufIdx++;
    }
    // \n triggers sending
    if (inputChar == '\n')
    {
      Serial.print("Sending: \"");
      for (uint8_t i = 0; i < inputBufIdx; i++)
      {
        Serial.print(inputBuf[i]);
      }
      Serial.println('\"');
      rf95.waitCAD();
      rf95.send(inputBuf, inputBufIdx);
      inputBufIdx = 0;
      rf95.waitPacketSent();
      Serial.println("Sent successfully!");
    }
  }
  if (rf95.recv(outputBuf, &outputBufLen))
  {
    Serial.print("Received: \"");
    for (uint8_t i = 0; i < outputBufLen; i++)
    {
      Serial.print(outputBuf[i]);
    }
    Serial.println('\"');
    outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
  }
}
