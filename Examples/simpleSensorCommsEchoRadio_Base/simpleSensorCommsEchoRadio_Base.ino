// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 902.5
#define RH_RF95_MAX_MESSAGE_LEN 128

RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t inputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t inputBufIdx = 0;
uint8_t outputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
char inputChar = 0;

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

  Serial.println("Feather LoRa Echo Test - Base!");

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
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(500000);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(15, false);
}

void loop() {
  // Transmit a string!
  if (Serial.available())
  {
    inputChar = Serial.read();
    Serial.print(char(inputChar));
    // Add chars to ignore to this.
    if ((inputChar != '\n'
         && inputChar != '\r')
        && inputBufIdx < RH_RF95_MAX_MESSAGE_LEN)
    {
      inputBuf[inputBufIdx] = inputChar;
      inputBufIdx++;
    }
    // Special start key
    if (inputChar == '$')
    {
      inputBuf[inputBufIdx] = 27;
      inputBufIdx++;
    }
    // \n triggers sending
    if (inputChar == '\n')
    {
      Serial.print("Sending: \"");
      for (uint8_t i = 0; i < inputBufIdx; i++)
      {
        Serial.print(char(inputBuf[i]));
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
      Serial.print(char(outputBuf[i]));
    }
    Serial.println('\"');
    outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
  }
}
