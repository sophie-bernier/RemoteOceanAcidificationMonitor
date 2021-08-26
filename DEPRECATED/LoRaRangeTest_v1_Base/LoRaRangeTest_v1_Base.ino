/**
 * @file LoRaRangeTest_v1_Base.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Simpler version of LoRaRangeTest_Base without logging or relying on external libraries.
 * @version 0.1
 * @date 2021-06
 * 
 * @copyright Copyright (c) 2021
 * 
 * @warning Never operate radio devices without either a suitable antenna (or other load) attached to the active transmitter. This will permanently damage the RF electronics, leading to a total loss of function. 
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define USB_SERIAL_BAUD  115200
#define HW_SERIAL_1_BAUD 9600

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3

// The default transmitter power is 13dBm, using PA_BOOST.
// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
// you can set transmitter powers from 5 to 23 dBm:
#define RFM95_DFLT_FREQ_MHz     902.5
#define RFM95_DFLT_SF           7
#define RFM95_DFLT_TX_POWER_dBm 15
#define RFM95_DFLT_BW_Hz        500000

#define RH_RF95_MAX_MESSAGE_LEN 128
#define RH_RELIABLE_DATAGRAM_ADDR 0xBB

#define USE_RH_RELIABLE_DATAGRAM true

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHReliableDatagram rhReliableDatagram(rf95, RH_RELIABLE_DATAGRAM_ADDR);

uint8_t txBuf[RH_RF95_MAX_MESSAGE_LEN] = "Hello World?";
uint8_t txBufIdx = 12;
uint8_t txSrcAddr;
uint8_t txDestAddr;
uint8_t txId;
uint8_t txFlags;

uint8_t rxBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
uint8_t rxSrcAddr;
uint8_t rxDestAddr;
uint8_t rxId;
uint8_t rxFlags;

uint8_t inputChar = 0;
uint32_t txIntervalMillis = 5*1000;
uint32_t currentMillis = 0;
uint32_t prevTxMillis = 0;

void setup()
{
  digitalWrite(10, HIGH);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1);
  }
  Serial1.begin(HW_SERIAL_1_BAUD);
  while (!Serial1)
  {
    delay(1);
  }

  delay(100);

  Serial.println("Feather LoRa Range Test - Endpoint!");

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

  #if (USE_RH_RELIABLE_DATAGRAM > 0)
  while (!rhReliableDatagram.init())
  {
    Serial.println("Manager init failed");
    while (1);
  }
  Serial.println("Manager init OK!");
  #endif // USE_RH_RELIABLE_DATAGRAM

  if (!rf95.setFrequency(RFM95_DFLT_FREQ_MHz))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RFM95_DFLT_FREQ_MHz);
  
  rf95.setSpreadingFactor(RFM95_DFLT_SF);
  rf95.setSignalBandwidth(RFM95_DFLT_BW_Hz);
  rf95.setTxPower(RFM95_DFLT_TX_POWER_dBm, false);
}

void loop() {
  currentMillis = millis();
  /*
  if ((currentMillis - prevTxMillis) > txIntervalMillis)
  {
    Serial.println("Attempting to transmit.");
    rf95.waitCAD();
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    if (rhReliableDatagram.sendtoWait(txBuf, txBufIdx, 0xBB))
    {
      Serial.println("Sent successfully & acknowleged!");
    }
    #else // USE_RH_RELIABLE_DATAGRAM
    rf95.send(txBuf, txBufIdx);
    rf95.waitPacketSent();
    Serial.println("Sent successfully!");
    #endif // USE_RH_RELIABLE_DATAGRAM
    prevTxMillis = currentMillis;
  }
  */
  if (
      #if (USE_RH_RELIABLE_DATAGRAM > 0)
      rhReliableDatagram.recvfromAckTimeout(rxBuf,
                                     &rxBufLen,
                                     500,
                                     &rxSrcAddr,
                                     &rxDestAddr,
                                     &rxId,
                                     &rxFlags)
      #else // USE_RH_RELIABLE_DATAGRAM
      rf95.recv(rxBuf, &rxBufLen)
      #endif  // USE_RH_RELIABLE_DATAGRAM
     )
  {
    Serial.print("Received: \"");
    for (uint8_t i = 0; i < rxBufLen; i++)
    {
      Serial.print(char(rxBuf[i]));
      Serial1.write(rxBuf[i]);
    }
    Serial.println('\"');
    rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
  }
}
