
/**
 * @file simpleSensorCommsEchoRadio_Endpoint.ino
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <sensors.h>
#include <loraPoint2PointProtocolLightweight.h>
#include "wiring_private.h" // Required for pinPeripheral function.

/**
 * @brief Enable direct sequence spreading spectrum
 * 
 */
#define DEBUG_ENABLE_DSSS false
#define DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT false
#define ENABLE_ACK false

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

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 903.0
#define RH_RF95_MAX_MESSAGE_LEN 255

#if (ENABLE_ACK == true)
#define ACK_LEN 2
#endif // ENABLE_ACK

#if DEBUG_ENABLE_DSSS
#define FREQ_CHANGE_INTERVAL_MS 1200
#endif // DEBUG_ENABLE_DSSS

#define USB_SERIAL_BAUD     115200
#define SEAPHOX_SERIAL_BAUD 4800
#define PROCV_SERIAL_BAUD   9600

#define SEAPHOX_SERIAL Serial1
#define PROCV_SERIAL   Serial2

#define DEBUG_ENABLE_DSSS false
#define DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT false

RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t seaphoxBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t seaphoxBufIdx = 1;
uint8_t procvBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t procvBufIdx = 1;
uint8_t outputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t outputBufLen = RH_RF95_MAX_MESSAGE_LEN;
uint32_t outputBufCksum = 0;
#if (ENABLE_ACK == true)
uint8_t ackBuf[ACK_LEN] = {msgType_ack, 0};
#endif // ENABLE_ACK
uint32_t inputBufCksum = 0;
uint32_t ledMillis = 0;
uint8_t rspBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t rspBufLen = 1;
bool procvDone = true;
bool seaphoxDone = true;
bool ledOn = false;

/**
 * @brief forwardUartToRadio
 * 
 * @param hwSerial 
 * @param inputBuffer 
 * @param inputBufIdx 
 * @param inputBufDone 
 * @param sensor 
 */
void forwardUartToRadio (Uart & hwSerial, uint8_t * & inputBuffer, uint8_t & inputBufIdx, bool & inputBufDone, sensors_t sensor);

/**
 * @brief setup function
 * 
 */
void setup()
{
  pinMode(13, OUTPUT);
  
  digitalWrite(10, HIGH);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1);
  }
  SEAPHOX_SERIAL.begin(SEAPHOX_SERIAL_BAUD);
  while (!SEAPHOX_SERIAL)
  {
    delay(1);
  }
  PROCV_SERIAL.begin(PROCV_SERIAL_BAUD);
  while (!PROCV_SERIAL)
  {
    delay(1);
  }
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
  delay(100);

  Serial.println("Feather LoRa Echo Test - Endpoint!");

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
  /*
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
  digitalWrite(16, LOW);
  */
}

void loop() {
  // blink LED
  if ((millis() - ledMillis) > 2000)
  {
    digitalWrite(13, ledOn);
    ledOn = !ledOn;
    ledMillis = millis();
  }
  
  #if DEBUG_ENABLE_DSSS
  rf95.advanceFrequencySequence(false, FREQ_CHANGE_INTERVAL_MS);
  #endif // DEBUG_ENABLE_DSSS
  
  // Transmit a string!
  forwardUartToRadio(SEAPHOX_SERIAL, seaphoxBuf, seaphoxBufIdx, seaphoxDone, sensor_seapHOx);
  forwardUartToRadio(PROCV_SERIAL, procvBuf, procvBufIdx, procvDone, sensor_proCV);
  /*
  digitalWrite(16, HIGH);
  */
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
      case msgType_seaphoxDataReq:
        for (uint8_t i = 1; i < outputBufLen; i++)
        {
          Serial.print(char(outputBuf[i]));
          SEAPHOX_SERIAL.print(char(outputBuf[i]));
        }
        break;
      case msgType_procvDataReq:
        for (uint8_t i = 1; i < outputBufLen; i++)
        {
          Serial.print(char(outputBuf[i]));
          PROCV_SERIAL.print(char(outputBuf[i]));
        }
        break;
      case msgType_dataReq:
        #if DEBUG_ENABLE_DSSS
        Serial.println("Starting hopping.");
        rspBuf[0] = msgType_dataRsp;
        rf95.send(rspBuf, rspBufLen);
        rf95.waitPacketSent();
        rf95.advanceFrequencySequence(true, FREQ_CHANGE_INTERVAL_MS);
        break;
        #endif // DEBUG_ENABLE_DSSS
        break;
      default:
        Serial.print("BAR");
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
    #if ENABLE_ACK
    if (outputBuf[0] != msgType_ack)
    {
      ackBuf[1] = outputBuf[0];
      rf95.send(ackBuf, ACK_LEN);
      inputBufCksum += outputBuf[0];
    }
    #endif // ENABLE_ACK
  }
  /*
  digitalWrite(16, LOW);
  */    
}

void forwardUartToRadio (Uart & hwSerial, uint8_t * inputBuf, uint8_t & inputBufIdx, bool & inputBufDone, sensors_t sensor)
{
  uint8_t inputChar;
  /*
  digitalWrite(14, HIGH);
  */
  if (hwSerial.available())
  {
    inputBufDone = false;
  }
  while(inputBufDone == false && hwSerial.available())
  {
    inputChar = hwSerial.read();
    switch (inputChar)
    {
      case '\n': // terminate and exit
      case '\r':
        Serial.print(char(inputChar));
        inputBufDone = true;
      case ' ': // ignore
      case '*':
        break;
      default:
        Serial.print(char(inputChar));
        inputBuf[inputBufIdx] = inputChar;
        inputBufCksum += (uint32_t)(inputChar);
        inputBufIdx++;
    }
  }
  /*
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);
  */
  if (inputBufIdx > 1 
      && inputBufDone == true)
  {
    Serial.print(": TX ");
    if (sensor == sensor_seapHOx)
    {
      inputBuf[0] = msgType_seaphoxDataRsp;
    }
    else if (sensor == sensor_proCV)
    {
      inputBuf[0] = msgType_procvDataRsp;
    }
    else
    {
      inputBuf[0] = msgType_dataRsp;
    }
    Serial.print(msgTypeNames[inputBuf[0]]);
    Serial.print(" (type #");
    Serial.print(inputBuf[0], DEC);
    Serial.print(") from");
    if (sensor == sensor_seapHOx)
    {
      Serial.print(" seaphox");
    }
    else if (sensor == sensor_proCV)
    {
      Serial.print(" procv");
    }
    else
    {
      Serial.print(" other");
    }
    rf95.waitCAD();
    rf95.send(inputBuf, inputBufIdx);
    inputBufIdx = 1;
    rf95.waitPacketSent();
    Serial.println(" sent");
    Serial.print("inputBufCksum = ");
    Serial.println(inputBufCksum);
  }
  /*
  digitalWrite(15, LOW);
  */
}

void SERCOM1_Handler() // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
