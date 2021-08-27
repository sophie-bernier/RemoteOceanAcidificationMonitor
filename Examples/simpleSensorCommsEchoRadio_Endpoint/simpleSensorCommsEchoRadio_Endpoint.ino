
/**
 * @file simpleSensorCommsEchoRadio_Endpoint.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Radio messages from simpleSensorCommsEchoRadio_Base.ino are echoed to either the seapHOx (on serial port 1 - the default hardware serial) or the CO2 pro-CV (on serial port 2 - using pin 10 as TXD and pin 11 as TXD).
 * @version 0.1
 * @date 2021-06-09
 * 
 * @copyright Copyright (c) 2021
 * 
 * @warning Never operate radio devices without either a suitable antenna (or other load) attached to the active transmitter. This will permanently damage the RF electronics, leading to a total loss of function. 
 *
 * @section Interfaces
 * @subsection Sensor interface
 * The SeapHOx and proCV both have an RS232 serial interface.
 * The RS-232 interfaces operate between -12V (1) and 12V (0), outside the voltage tolerance of the Feather.
 * Therefore, you must connect serial1 and serial2 to the sensors through a level shifter, such as the MAX232 or MAX3232.
 * To keep up with the radio, you must set the SeapHOx to *4800 baud* and the ProCV to *9600 baud*.
 * Both sensors use 8 data bits, 1 stop bit, and no parity bit.
 * 
 * Ignored characters:
 * - Space (' ')
 * - Asterisk ('*')
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <sensors.h>
#include <loraPoint2PointProtocolLightweight.h>
#include "wiring_private.h" // Required for pinPeripheral function.

/**
 * @brief Enable direct sequence spread spectrum (DSSS).
 * 
 * To limit interference, regulatory authorities (such as the FCC) limit the length of time a radio dwells on a single frequency.
 * Direct sequence spread spectrum reduces the dwell time by periodically switching the frequency that two or more radios are transmitting and recieving on, following a repeating, fixed cycle of randomly ordered frequency channels.
 * Frequency hopping in general is useful as it reduces the chance that other transmitters will interfere with yours and vice-versa but still allows high data rates.
 * DSSS in particular is useful as it is a simple way of implementing frequency hopping - the primary obstacle lies in ensuring that all the units that wish to communicate with each other synchronized within the frequency hopping cycle.
 * In this application, synchronization is maintained through a 'join' message.
 * When the base's user wishes to exchange data with the endpoint, they command the base to send 'join' messages across all channels until the endpoint recieves one, at which time it will transmit a response to the base. When the base recieves the join response, and when the endpoint finishes transmitting said response, both units reset their frequency hopping sequence, ensuring that they are near perfectly aligned.
 * 
 * @warning Under heavy development. Use at your own risk.
 * 
 * To enable DSSS,
 * 1. Clone https://github.com/Sebastien-B/RadioHead_RFM95LoRaFHSS into your Arduino/libraries folder, if you have not already.
 * 2. In Arduino/libraries/RadioHead/RH_RF95.h, set ENABLE_RF95_FHSS to 3.
 * 3. Set DEBUG_ENABLE_DSSS to 'true'.
 * To disable DSSS,
 * 1. In Arduino/libraries/RadioHead/RH_RF95.h, set ENABLE_RF95_FHSS to 0.
 * 2. Set DEBUG_ENABLE_DSSS to 'false'.
 */
#define DEBUG_ENABLE_DSSS false

/**
 * @brief Enable frequency hopping spread spectrum (FHSS) controlled by the RFM95.
 * 
 * The RFM95 has built-in support for limited frequency hopping.
 * The RFM95's manufacturers intend this to be used when a single packet's transmission time exceeds the regulatory limit for frequency channel dwell time.
 * This is not useful for the settings used in the ROAM project, as all packets can be transmitted within the maximum permitted dwell time, but this option is preserved as it may be useful for future projects using lower data rates.
 * 
 * @warning Under development. Use at your own risk.
 * 
 * To enable interrupt-controlled FHSS,
 * 1. Clone https://github.com/Sebastien-B/RadioHead_RFM95LoRaFHSS into your Arduino/libraries folder, if you have not already.
 * 2. In Arduino/libraries/RadioHead/RH_RF95.h, set DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT to 1.
 * 3. Set DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT to 'true'.
 * To disable DSSS,
 * 1. In Arduino/libraries/RadioHead/RH_RF95.h, set ENABLE_RF95_FHSS to 0.
 * 2. Set DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT to 'false'.
 */
#define DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT false

#if (DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT && DEBUG_ENABLE_DSSS)
#error Enabling both DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT and DEBUG_ENABLE_DSSS may cause unintended behaviour.
#endif // DEBUG_ENABLE_FHSS_ON_RF95_INTERRUPT, DEBUG_ENABLE_DSSS

/**
 * @brief Enable sending an acknowledgment message including the message ID of the acknowledged message.
 * 
 * @todo Have the acknowledgment message send back the checksum of the message being acknowledged, so the recipient can associate the acknowledgment with the message being acknowledged.
 * 
 */
#define ENABLE_ACK false

/**
 * @brief Start the USB serial on startup and blocks until connection is achieved.
 * 
 */
#define ENABLE_USB_SERIAL true

#define NUM_UART_BUFFERS 2

/**
 * @brief Pin description number for PIO_SERCOM RX pin, set here to arduino pin 11 (D11).
 * 
 * Follow the example from variant.h.
 * Available pins and the corresponding available SERCOMs are listed in variant.cpp.
 * These files are typically located (on Windows) at:
 * C:\Users\[user]\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.7.3\variants\feather_m0
 * 
 */
#define PIN_SERIAL2_RX       (11ul)

/**
 * @brief Pin description number for PIO_SERCOM TX pin, set here to arduino pin 10 (D10).
 * 
 * Follow the example from variant.h.
 * Available pins and the corresponding available SERCOMs are listed in variant.cpp.
 * These files are typically located (on Windows) at:
 * C:\Users\[user]\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.7.3\variants\feather_m0
 * 
 */
#define PIN_SERIAL2_TX       (10ul)


#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // Pin 10 is SERCOM pad 2 of this channel.
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_0)    // Pin 11 is SERCOM pad 0 of this channel.

/**
 * @brief Construct Serial2 port.
 * 
 * This constructs a SERCOM object with pins belonging to the desired SERCOM's primary or alternate channel and the pads corresponding to those pins. 
 * The SAMD21 used on the Adafruit Feather M0 has 6 hardware SERCOM (serial communication) controllers.
 * Each pin on the Feather is connected to a pin on the M0. Some pins on the M0 are grouped into groups of 4, forming a useable SERCOM channel. Every SERCOM can be routed either to a primary or a secondary channel. Each pin in the channel is connected to a pad on the M0 die that can be multiplexed to one of several peripherals, such as the GPIO bank, the SERCOM, or the ADC input. The SERCOM itself can be configured either as an I2C, SPI, or UART port, and if it uses 2-wire UART, the RX and TX can be chosen from different pads. This is chosen when you declare the UART object. The available choices are declared in SERCOM.h.
 * TX can use the pin corresponding to UART_TX_PAD_0 or UART_TX_PAD_2
 * RX can use the pin corresponding to SERCOM_RX_PAD_0, SERCOM_RX_PAD_1, SERCOM_RX_PAD_3, or SERCOM_RX_PAD_4.
 * 
 * See SERCOM.h, variant.h, and variant.cpp for full details.
 * These files are typically located (on Windows) at:
 * C:\Users\[user]\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.7.3
 * 
 * @return Uart object referencing the new hardware serial port.
 */
Uart Serial2(&sercom1,
             PIN_SERIAL2_RX,
             PIN_SERIAL2_TX,
             PAD_SERIAL2_RX,
             PAD_SERIAL2_TX);

/**
 * @brief RFM95 SPI chip-select pin.
 * 
 * Chip select is active low.
 * 
 */
#define RFM95_CS  8

/**
 * @brief RFM95 SPI reset pin.
 * 
 * Reset is active low.
 * 
 */
#define RFM95_RST 4

/**
 * @brief RFM95 interrupt pin.
 * 
 * Interrupts are handled by the RadioHead RF95 driver.
 * 
 */
#define RFM95_INT 3

/**
 * @brief Default frequency in MHz.
 * 
 * Min: 900dBm, Max: 928dBm.
 */
#define RFM95_FREQ 903.0

/**
 * @brief Default bandwidth in Hz.
 * 
 * Permitted:
 * - 125kHz
 * - 250kHz
 * - 500kHz
 */
#define RFM95_BW 500000

/**
 * @brief Default transmission power in dBm.
 * 
 * Min: 2dBm, Max: 20dBm.
 * 
 */
#define RFM95_TX_POWER 2

/**
 * @brief Default spreading factor.
 * Spreading factor is the number of chirps or symbols per bit.
 * See section 4.1.1.2. of the Semtech SX1276 LoRA transciever datasheet for a complete explanation of the spreading factor.
 *
 * Min: 7, Max: 12
 */
#define RFM95_SF 7

/**
 * @brief Max message length supported by the RFM95 driver.
 * 
 */
#define RH_RF95_MAX_MESSAGE_LEN 255

/**
 * @brief SD card chip select pin.
 * 
 */
#define SD_CS 10

/**
 * @brief USB baud rate.
 * 
 */
#define USB_SERIAL_BAUD     115200

/**
 * @brief SeapHOx baud rate
 * 
 */
#define SEAPHOX_SERIAL_BAUD 4800

/**
 * @brief CO2 Pro CV baud rate.
 * 
 */
#define PROCV_SERIAL_BAUD   9600

/**
 * @brief Assign SeapHOx serial port.
 * 
 */
#define SEAPHOX_SERIAL Serial1

/**
 * @brief Assign proCV serial port.
 * 
 */
#define PROCV_SERIAL   Serial2

/**
 * @brief LED pin.
 * 
 */
#define LED_PIN 13

#if (ENABLE_ACK == true)
#define ACK_LEN 2
#endif // ENABLE_ACK

#if DEBUG_ENABLE_DSSS
#define FREQ_CHANGE_INTERVAL_MS 1200
#endif // DEBUG_ENABLE_DSSS

RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t seaphoxBuf[RH_RF95_MAX_MESSAGE_LEN][NUM_UART_BUFFERS];
uint8_t seaphoxBufIdx[NUM_UART_BUFFERS] = {1,1};
uint8_t seaphoxActiveBuf = 0;
uint8_t seaphoxDoneBuf = 0xFF;
uint8_t procvBuf[RH_RF95_MAX_MESSAGE_LEN][NUM_UART_BUFFERS];
uint8_t procvBufIdx[NUM_UART_BUFFERS] = {1,1};
uint8_t procvActiveBuf = 0;
uint8_t procvDoneBuf = 0xFF;
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

void forwardUartToRadio (Uart & hwSerial, uint8_t inputBuf[][NUM_UART_BUFFERS], uint8_t inputBufIdx[], uint8_t & activeInputBuf, uint8_t & doneInputBuf, sensors_t sensor);

/**
 * @brief setup function
 * 
 */
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(SD_CS, HIGH);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  #if (ENABLE_USB_SERIAL > 0)
  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1);
  }
  #endif // ENABLE_USB_SERIAL

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
  pinPeripheral(PIN_SERIAL2_TX, PIO_SERCOM);
  pinPeripheral(PIN_SERIAL2_RX, PIO_SERCOM);
  delay(100);

  Serial.println("Feather LoRa Echo Test - Endpoint");

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
  if (!rf95.setFrequency(RFM95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RFM95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  rf95.setSpreadingFactor(RFM95_SF);
  rf95.setSignalBandwidth(RFM95_BW);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(RFM95_TX_POWER, false);
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
    digitalWrite(LED_PIN, ledOn);
    ledOn = !ledOn;
    ledMillis = millis();
  }
  
  #if DEBUG_ENABLE_DSSS
  rf95.advanceFrequencySequence(false, FREQ_CHANGE_INTERVAL_MS);
  #endif // DEBUG_ENABLE_DSSS
  
  // Transmit a string!
  forwardUartToRadio(SEAPHOX_SERIAL, seaphoxBuf, seaphoxBufIdx, seaphoxActiveBuf, procvDoneBuf, sensor_seapHOx);
  forwardUartToRadio(PROCV_SERIAL, procvBuf, procvBufIdx, procvActiveBuf, procvDoneBuf, sensor_proCV);
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

void forwardUartToRadio (Uart & hwSerial, uint8_t inputBuf[][NUM_UART_BUFFERS], uint8_t inputBufIdx[], uint8_t & activeInputBuf, uint8_t & doneInputBuf, sensors_t sensor)
{
  uint8_t inputChar;
  /*
  digitalWrite(14, HIGH);
  */
  while(hwSerial.available() && doneInputBuf == 0xFF)
  {
    inputChar = hwSerial.read();
    switch (inputChar)
    {
      case '\n': // terminate and exit
      case '\r':
        Serial.print(char(inputChar));
        doneInputBuf = activeInputBuf;
        activeInputBuf = (activeInputBuf > 0) ? 0 : 1;
      case ' ': // ignore
      case '*':
        break;
      default:
        Serial.print(char(inputChar));
        inputBuf[inputBufIdx[activeInputBuf]][activeInputBuf] = inputChar;
        inputBufCksum += (uint32_t)(inputChar);
        inputBufIdx[activeInputBuf]++;
    }
  }
  /*
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);
  */
  if (doneInputBuf != 0xFF)
  {
    if (inputBufIdx[doneInputBuf] > 1)
    {
      Serial.print(": TX ");
      if (sensor == sensor_seapHOx)
      {
        inputBuf[0][doneInputBuf] = msgType_seaphoxDataRsp;
      }
      else if (sensor == sensor_proCV)
      {
        inputBuf[0][doneInputBuf] = msgType_procvDataRsp;
      }
      else
      {
        inputBuf[0][doneInputBuf] = msgType_dataRsp;
      }
      Serial.print(msgTypeNames[inputBuf[0][doneInputBuf]]);
      Serial.print(" (type #");
      Serial.print(inputBuf[0][doneInputBuf], DEC);
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
      Serial.print(" buffer ");
      Serial.print(doneInputBuf);
      rf95.waitCAD();
      rf95.send(inputBuf[doneInputBuf], inputBufIdx[doneInputBuf]);
      rf95.waitPacketSent();
      Serial.print(" sent, ");
      Serial.print("inputBufCksum = ");
      Serial.println(inputBufCksum);
      inputBufIdx[doneInputBuf] = 1;
    }
    doneInputBuf = 0xFF;
  }
  /*
  digitalWrite(15, LOW);
  */
}

void SERCOM1_Handler() // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
