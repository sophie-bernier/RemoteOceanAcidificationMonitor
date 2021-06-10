// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define USB_SERIAL_BAUD  115200

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

uint8_t txBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t txBufIdx = 0;
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

char inputChar = 0;

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1);
  }

  delay(100);

  Serial.println("Feather LoRa Range Test - Base!");

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
  // Transmit a string!
  while (Serial.available())
  {
    inputChar = Serial.read();
    Serial.print(char(inputChar));
    // Add chars to ignore to this.
    if ((inputChar != '\n'
         && inputChar != '\r')
        && txBufIdx < RH_RF95_MAX_MESSAGE_LEN)
    {
      txBuf[txBufIdx] = inputChar;
      txBufIdx++;
    }
    // Special start key
    if (inputChar == '$')
    {
      txBuf[txBufIdx] = 27;
      txBufIdx++;
    }
    // \n triggers sending
    if (inputChar == '\n')
    {
      Serial.print("Attempting to transmit: \"");
      for (uint8_t i = 0; i < txBufIdx; i++)
      {
        Serial.print(char(txBuf[i]));
      }
      Serial.println('\"');
      rf95.waitCAD();
      #if (USE_RH_RELIABLE_DATAGRAM > 0)
      if (rhReliableDatagram.sendtoWait(txBuf, txBufIdx, 0xEE) == true)
      {
        Serial.println("Sent successfully & acknowleged!");
      }
      txBufIdx = 0;
      #else // USE_RH_RELIABLE_DATAGRAM
      rf95.send(txBuf, txBufIdx);
      txBufIdx = 0;
      rf95.waitPacketSent();
      #endif  // USE_RH_RELIABLE_DATAGRAM
      Serial.println("Sent successfully!");
    }
  }
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
    }
    Serial.println('\"');
    rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
  }
}
