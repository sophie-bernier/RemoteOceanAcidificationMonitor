
/**
 * @file simpleSensorCommsEchoRadio_Endpoint.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Radio messages from simpleSensorCommsEchoRadio_Endpoint.ino are echoed to the serial port and vice-versa.
 * @version 0.1
 * @date 2021-06-09
 * 
 * @copyright Copyright (c) 2021
 * 
 * @warning Never operate radio devices without either a suitable antenna (or other load) attached to the active transmitter. This will permanently damage the RF electronics, leading to a total loss of function. 
 * 
 * @section Interfaces
 * @subsection User interface
 * Operates at 115200 baud, 8 data bits, 1 stop bit, and no parity bit.
 * Interface using a standard USB A to USB Micro-B male-male cable.
 * Control using a terminal emulator such as TeraTerm, Termite, or the built-in Arduino terminal emulator (on Windows). Set it to append a newline ('\n') with every 'enter' press.
 * 
 * Ignored characters:
 * - Space (' ')
 * - Asterisk ('*')
 * Control characters:
 * - '%' : Send this and all following messages to the CO2 Pro CV.
 * - '$' : Send this and all following messages to the SeapHOx.
 * - '#' : This character is replaced with the 'ESC' (0x1B) character. Used to wake up the CO2 Pro CV user interface.
 * - '&' : This character is replaced with the carriage return ('\r') character. Used to confirm numerical inputs for the CO2 Pro CV and to confirm all commands for the SeapHOx.
 * - '\n' or '\r' : Send the current input buffer. 
 * All other characters are appended to the input buffer.
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>
#include <sensors.h>
#include <loraPoint2PointProtocolLightweight.h>

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
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1); 
  }

  delay(100);

  Serial.println("Feather LoRa Echo Test - Base");

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
