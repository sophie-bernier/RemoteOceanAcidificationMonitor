// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.
#include <SPI.h>
#include <RH_RF95.h>
#include <sensors.h>
#include <loraPoint2PointProtocolLightweight.h>

#define DEBUG_ENABLE_DSSS false
#define DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT false
#define ENABLE_ACK false

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 903.0
#define RH_RF95_MAX_MESSAGE_LEN 255

#if ENABLE_ACK
#define ACK_LEN 2
#endif // ENABLE_ACK

#if DEBUG_ENABLE_DSSS
#define FREQ_CHANGE_INTERVAL_MS 1200
#endif // DEBUG_ENABLE_DSSS

RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t inputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t inputBufIdx = 1;
uint32_t inputBufCksum = 0;
uint8_t outputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
uint32_t outputBufCksum = 0;
#if ENABLE_ACK
uint8_t ackBuf[ACK_LEN] = {msgType_ack, 0};
#endif // ENABLE_ACK
char inputChar = 0;
bool done = true;
#if DEBUG_ENABLE_DSSS
bool joined = false;
bool joining = false;
bool joinTxComplete = false;
#endif // DEBUG_ENABLE_DSSS
sensors_t sendTo = sensor_none;

void setup()
{
  digitalWrite(10, HIGH);

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
  
  #if DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT
  rf95.setFreqHoppingPeriod(10);
  Serial.println("Started hopping...");
  #endif // DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT

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
  rf95.setTxPower(2, false);
}

void loop() {
  #if DEBUG_ENABLE_DSSS
  if (joined == false)
  {
    joinTxComplete = !rf95.advanceFrequencySequence(false, FREQ_CHANGE_INTERVAL_MS/3);
  }
  else
  {
    rf95.advanceFrequencySequence(false, FREQ_CHANGE_INTERVAL_MS);
  }
  #endif // DEBUG_ENABLE_DSSS
  // Transmit a string!
  if (Serial.available())
  {
    done = false;
  }
  while (done == false && Serial.available())
  {
    inputChar = Serial.read();
    Serial.print(char(inputChar));
    switch (inputChar)
    {
      case '\n': // terminate and exit
      case '\r':
        done = true;
      case ' ': // ignore
      case '*':
        break;
      #if DEBUG_ENABLE_DSSS
      case '@':
        sendTo = sensor_none;
        joining = true;
        joined = false;
        done = true;
        break;
      #endif // DEBUG_ENABLE_DSSS
      case '$': // send this and subsequent messages to the seaphox
        sendTo = sensor_seapHOx;
        break;
      case '%': // send this and subsequent messages to the proCV
        sendTo = sensor_proCV;
        break;
      default:
        inputBuf[inputBufIdx] = inputChar;
        inputBufCksum += (uint32_t)(inputChar);
        inputBufIdx++;
        break;
      case '&':
        inputBuf[inputBufIdx] = '\r';
        inputBufCksum += (uint32_t)('\r');
        inputBufIdx++;
        break;
      case '#':
        inputBuf[inputBufIdx] = 0x1B;
        inputBufCksum += (uint32_t)(0x1B);
        inputBufIdx++;
        break;
    }
  }
  if ((inputBufIdx > 1
       && done == true)
      #if DEBUG_ENABLE_DSSS
      || (joining == true
          && joinTxComplete == false))
      #else // DEBUG_ENABLE_DSSS
      )
      #endif // DEBUG_ENABLE_DSSS
  {
    Serial.println(": TX ");
    switch (sendTo)
    {
      case sensor_proCV:
        inputBuf[0] = msgType_procvDataReq;
        break;
      case sensor_seapHOx:
        inputBuf[0] = msgType_seaphoxDataReq;
        break;
      case sensor_none:
      default:
      #if DEBUG_ENABLE_DSSS
        Serial.println("JOINING");
        joinTxComplete = true;
        inputBufIdx = 1;
      #endif // DEBUG_ENABLE_DSSS
        inputBuf[0] = msgType_dataReq;
        break;
    }
    Serial.print(msgTypeNames[inputBuf[0]]);
    Serial.print(" (type #");
    Serial.print(inputBuf[0], DEC);
    Serial.print(") ");
    rf95.waitCAD();
    rf95.send(inputBuf, inputBufIdx);
    inputBufIdx = 1;
    rf95.waitPacketSent();
    Serial.println("sent");
    Serial.print("inputBufCksum = ");
    Serial.println(inputBufCksum);
  }
  if (rf95.recv(outputBuf, &outputBufLen))
  {
    Serial.println();
    Serial.print("RX ");
    Serial.print(msgTypeNames[outputBuf[0]]);
    Serial.print(" (type #");
    Serial.print(outputBuf[0], DEC);
    Serial.print("): ");
    switch (outputBuf[0])
    {
      #if ENABLE_ACK
      case msgType_ack:
        Serial.print("to ");
        Serial.print(msgTypeNames[outputBuf[1]]);
        Serial.print(" (type #");
        Serial.print(outputBuf[1], DEC);
        Serial.print(")");
        break;
      #endif // ENABLE_ACK
      case msgType_dataRsp:
        #if DEBUG_ENABLE_DSSS
        rf95.advanceFrequencySequence(true, FREQ_CHANGE_INTERVAL_MS);
        Serial.println("Starting hopping.");
        joined = true;
        joining = false;
        break;
        #endif // DEBUG_ENABLE_DSSS
      default:
        for (uint8_t i = 1; i < outputBufLen; i++)
        {
          Serial.print(char(outputBuf[i]));
        }
        break;
    }
    Serial.println();
    for (uint8_t i = 1; i < outputBufLen; i++)
    {
      outputBufCksum += uint32_t(outputBuf[i]);
    }
    outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
    Serial.print("outputBufCksum = ");
    Serial.println(outputBufCksum);
    outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
    #if ENABLE_ACK
    if (outputBuf[0] != msgType_ack)
    {
      ackBuf[1] = outputBuf[0];
      rf95.send(ackBuf, ACK_LEN);
      inputBufCksum += outputBuf[0];
    }
    #endif // ENABLE_ACK
  }
}
