/**
 * @file LoRaInterfaceForwardingTest_Base.ino
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Echoes serial communication over UART and vice versa. Sensor interface currently broken pending upgrades to the protocol.
 * @version 0.1
 * @date 2021-07
 * 
 * @copyright Copyright (c) 2021
 *
 * @warning This sketch is currently broken! See Problem Solving 7 (ps7). This sketch has the potential to enter a race condition in which the endpoint transmits before the base is ready to recieve it. This is an issue with the lack of defined transmission windows in this program.
 *
 * @warning Never operate radio devices without either a suitable antenna (or other load) attached to the active transmitter. This will permanently damage the RF electronics, leading to a total loss of function. 
 * 
 * This is the client, with SD logging and an OLED display.
 * It is based on LoRaRangeTest, using RHReliableDatagram and loraPoint2PointProtocol.
 * 
 */

#include <SPI.h>
#include <Wire.h> 
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <loraPoint2PointProtocol.h>

#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define BUTTON_SETTING_SELECT BUTTON_A
#define BUTTON_VALUE_SELECT   BUTTON_B
#define BUTTON_ENTER          BUTTON_C

#define SD_CS     10
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RH_RELIABLE_DATAGRAM_ADDR 0xBB

#define USB_SERIAL_BAUD 115200

#define DISPLAY_X        128
#define DISPLAY_Y        32
#define DISPLAY_CHAR_X   6
#define DISPLAY_CHAR_Y   8
#define DISPLAY_RX_START 65
#define DISPLAY_TX_START 0
#define DISPLAY_RX_LEN   DISPLAY_X - DISPLAY_RX_START
#define DISPLAY_TX_LEN   60
#define DISPLAY_RX_CHARS 10
#define DISPLAY_TX_CHARS 10

//--------------------------------
// Callback function declarations
//--------------------------------

void txInd (uint8_t const * txBuf,
            uint8_t const bufLen,
            uint8_t const destAddr,
            bool ack);
void rxInd (message_t const & rxMsg);
void linkChangeInd (spreadingFactor_t const newSpreadingFactor,
                    signalBandwidth_t const newSignalBandwidth,
                    frequencyChannel_t const newFrequencyChannel,
                    int8_t const newTxPower);

//---------
// Classes
//---------

userCallbacks_t callbacks = {txInd, rxInd, linkChangeInd};

loraPoint2Point point2point(RH_RELIABLE_DATAGRAM_ADDR,
                            RFM95_CS,
                            RFM95_INT,
                            RFM95_RST,
                            callbacks);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
File dataFile;
String dataFileName = "datalog.csv";
String dataFileHeader = "Timestamp,Source Address,Destination Address,Message ID,Message Flags,Acknowleged,Message,spreadingFactor,signalBandwidth,frequencyChannel,txPower";

//-----------------
// Local functions
//-----------------

void updateSettingDisplay ();
void readButtons ();
void processButtons ();

//------------
// Main Setup
//------------

void setup()
{
  Serial.begin(USB_SERIAL_BAUD);
  while (!Serial)
  {
    delay(1);
  }
  Serial.println("Feather LoRa Sensor Interface Forwarding Test - Base");
  
  digitalWrite(SD_CS, HIGH); // tie SD high
  digitalWrite(RFM95_CS, HIGH); // tie radio high

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Display initialization failed.");
  }
  else
  {
    Serial.println("Display initialized.");
  }
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  
  display.print("Radio:    ");
  display.display();
  if (!point2point.setupRadio())
  {
    display.println("failed");
    display.display();
    while(1);
  }
  else
  {
    display.println("OK");
    display.display();
  }

  Serial.print("Initializing SD card...");
  display.print("SD open:  ");
  display.display();
  if (!SD.begin(SD_CS))
  {
    Serial.println("Card failed or not present");
    display.println("failed");
    display.display();
  }
  else
  {
    Serial.println("Card initialized.");
    display.println("OK");
    display.display();
  }

  display.print("SD write: ");
  display.display();
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(dataFileHeader);
    Serial.println("Header written to SD card.");
    display.println("OK");
    display.display();
  }
  else
  {
    Serial.println("SD card write failed.");
    display.println("failed");
    display.display();
  }
  dataFile.close();

  display.println("Starting");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.drawFastVLine(62,  0, 32, 1);
  display.drawFastHLine(61,  0,  3, 1);
  display.drawFastHLine(61, 31,  3, 1);
  /* Test pattern, do not need.
  display.fillRect( 0,  0, 16, 16, 1);
  display.fillRect(16,  0, 16, 16, 0);
  display.fillRect( 0  16, 16, 16, 0);
  display.fillRect(16, 16, 16, 16, 1);
  display.drawRect( 0,  0, 32, 32, 1);
  */
  display.display();
  //point2point.startHeartbeats();
}

//-----------
// Main Loop
//-----------

uint32_t prevMillis = 0;
uint32_t currentMillis = 0;
uint32_t lastAckMillis = 0;
uint32_t prevLinkChangeMillis = 0;
uint32_t prevButtonScanMillis = 0;
bool timeUp = false;

spreadingFactor_t  spreadingFactor = RFM95_DFLT_SPREADING_FACTOR;
signalBandwidth_t  signalBandwidth = RFM95_DFLT_SIGNAL_BANDWIDTH;
frequencyChannel_t frequencyChannel = RFM95_DFLT_FREQ_CHANNEL;
int8_t             txPower = RFM95_DFLT_TX_POWER_dBm;
uint8_t varToIncrement = 0;
uint8_t setting = 0;
  uint8_t const maxSettingValue [] = {uint8_t(NUM_spreadingFactors),
                                    uint8_t(NUM_signalBandwidths),
                                    uint8_t(NUM_frequencyChannels),
                                    MAX_txPower};
uint8_t settingSelectCount = 0;
uint8_t valueSelectCount = 0;
uint8_t enterCount = 0;
uint8_t noPressCount = 0;
bool settingSelect = false;
bool valueSelect = false;
bool enter = false;
bool prevSettingSelect = false;
bool prevValueSelect = false;
bool prevEnter = false;
bool valueChanged = false;
bool suspendRadio = false;

void loop()
{  
  // Transmit a string!
  
  currentMillis = millis();
  //Serial.print("currentMillis = ");
  //Serial.println(currentMillis);
  //Serial.print("prevMillis = ");
  //Serial.println(prevMillis);
  //Serial.print("currentMillis - prevMillis = ");  
  //Serial.println(currentMillis - prevMillis);
  
  
  if ((currentMillis - prevButtonScanMillis) > 10)
  {
    prevButtonScanMillis = currentMillis; // debounce
    prevSettingSelect = settingSelect;
    prevValueSelect = valueSelect;
    prevEnter = enter;
    settingSelect = !digitalRead(BUTTON_SETTING_SELECT);
    valueSelect = !digitalRead(BUTTON_VALUE_SELECT);
    enter = !digitalRead(BUTTON_ENTER);
    if (settingSelect)
    {
      noPressCount = 0;
      settingSelectCount++;
      Serial.print("A:");
      Serial.println(settingSelectCount);
    }
    if (valueSelect)
    { 
      noPressCount = 0;
      valueSelectCount++;
      Serial.print("B:");
      Serial.println(valueSelectCount);
    }
    if (enter)
    {
      noPressCount = 0;
      enterCount++;
      Serial.print("C:");
      Serial.println(enterCount);
    }
    if (!(enter || valueSelect || enter))
    {
      noPressCount++;
      //Serial.print("N:");
      //Serial.println(noPressCount);
    }
    if (noPressCount == 2)
    {
      noPressCount++;
      settingSelectCount = 0;
      valueSelectCount = 0;
      enterCount = 0;
      noPressCount = 0;
    }
    if (noPressCount == 10)
    {
      noPressCount = 0;
    }
    if (settingSelectCount == 2)
    {
      settingSelectCount++;
      valueSelectCount = 0;
      enterCount = 0;
      noPressCount = 0;
      setting++;
      if (setting > 3)
      {
        setting = 0;
      }
    }
    if (valueSelectCount == 2)
    {
      valueSelectCount++;
      settingSelectCount = 0;
      enterCount = 0;
      noPressCount = 0;
      valueChanged = true;
      switch (setting)
      {
        case 0:
          spreadingFactor++;
          break;
        case 1:
          signalBandwidth++;
          break;
        case 2:
          frequencyChannel++;
          break;
        case 3:
          txPower++;
          if (txPower > MAX_txPower)
          {
            txPower = MIN_txPower;
          }
          break;
      }
    }
    if (enterCount == 2)
    {
      enterCount++;
      settingSelectCount = 0;
      valueSelectCount = 0;
      noPressCount = 0;
      point2point.linkChangeReq(0xEE, spreadingFactor, signalBandwidth, frequencyChannel, txPower);
      /*
      spreadingFactor = point2point.getSpreadingfactor();
      signalBandwidth = point2point.getSignalBandwidth();
      frequencyChannel = point2point.getFrequencyChannel();
      txPower = point2point.getTxPower();
      */
      /*
      switch (setting)
      {
        case 0:
          point2point.setSpreadingFactor(spreadingFactor);
          break;
        case 1:
          point2point.setBandwidth(signalBandwidth);
          break;
        case 2:
          point2point.setFrequencyChannel(frequencyChannel);
          break;
        case 3:
          point2point.setTxPower(txPower);
          break;
      }
      */
      valueChanged = false;
    }
    display.fillRect(DISPLAY_RX_START, DISPLAY_CHAR_Y*3, DISPLAY_RX_LEN, DISPLAY_CHAR_Y, 0);
    display.setCursor(DISPLAY_RX_START, DISPLAY_CHAR_Y*3);
    switch (setting)
    {
      case 0:
        display.print("S");
        display.print(int(spreadingFactor));
        break;
      case 1:
        display.print("B");
        display.print(int(signalBandwidth));
        break;
      case 2:
        display.print("C");
        display.print(int(frequencyChannel));
        break;
      case 3:
        display.print("P");
        display.print(txPower);
        break;
    }
    if (valueChanged)
    {
      display.print("*");
    }
    else
    {
      display.print(" ");
    }
    display.print(int(point2point.getPacketErrorFraction()*100));
    display.print("%");
    display.display();
  }
  if (suspendRadio == false)
  {
    /*
    if ((currentMillis - prevMillis) > 5000)
    {
      point2point.setTxMessage((uint8_t*)("foobar"), 6);
      prevMillis = currentMillis;
      timeUp = true;
    }
    */
    
    if (point2point.buildStringFromSerial(&Serial) || timeUp)
    {
      point2point.serviceTx(0xEE);
      timeUp = false;
    }
    /*
    if ((currentMillis - prevLinkChangeMillis) > 21010)
       //(false)
    {
      point2point.linkChangeReq(0xEE, spreadingFactor, signalBandwidth, frequencyChannel, txPower);
      prevLinkChangeMillis = currentMillis;
      switch (varToIncrement)
      {
      case 0:
      spreadingFactor = spreadingFactor_sf8;
      //point2point.setSpreadingFactor(spreadingFactor_sf8);
      varToIncrement++;
      break;
      case 1:
      signalBandwidth = signalBandwidth_250kHz;
      //point2point.setBandwidth(signalBandwidth_250kHz);
      varToIncrement++;
      break;
      case 2:
      frequencyChannel = frequencyChannel_500kHz_Uplink_1;
      //point2point.setFrequencyChannel(frequencyChannel_500kHz_Uplink_1);
      varToIncrement++;
      break;
      case 3:
      txPower++;
      //point2point.setTxPower(16);
      varToIncrement++;
      break;
      default:
      break;
      }
    }
    */
    point2point.serviceRx(); 
  }
}

//-------------------------------
// Callback function definitions
//-------------------------------

void txInd (uint8_t const * txBuf,
            uint8_t const bufLen,
            uint8_t const destAddr,
            bool ack)
{
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile)
  {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(RH_RELIABLE_DATAGRAM_ADDR, HEX);
    dataFile.print(",");
    dataFile.print(destAddr, HEX);
    dataFile.print(",");
    dataFile.print((int)(txBuf[0]));
    dataFile.print(",");
    dataFile.print(",");
    dataFile.print(ack);
    dataFile.print(",");
    for (uint8_t i = 1; i < bufLen; i++)
    {
      dataFile.print(char(txBuf[i]));
    }
    dataFile.print(",");
    dataFile.print(spreadingFactor);
    dataFile.print(",");
    dataFile.print(signalBandwidth);
    dataFile.print(",");
    dataFile.print(frequencyChannel);
    dataFile.print(",");
    dataFile.print(txPower);
    dataFile.println();
    Serial.println("TX data written to SD card.");
  }
  else
  {
    Serial.println("SD card write failed.");
  }
  dataFile.close();
  display.fillRect(DISPLAY_TX_START, 0, DISPLAY_TX_LEN, DISPLAY_Y, 0);
  display.setCursor(DISPLAY_TX_START, 0);
  display.print("To ");
  display.print(destAddr, HEX);
  display.print("h ");
  if (ack == true)
  {
    lastAckMillis = currentMillis;
    display.print("ACK");
  }
  else
  {
    display.print("NAK");
  }
  display.setCursor(DISPLAY_TX_START, DISPLAY_CHAR_Y);
  for (uint8_t i = 0; (i < bufLen) && (i < DISPLAY_RX_CHARS); i++)
  {
    display.print(char(txBuf[i]));
  }
  display.setCursor(DISPLAY_TX_START, DISPLAY_CHAR_Y*2);
  display.print("TX:");
  display.print(currentMillis/1000);
  display.setCursor(DISPLAY_TX_START, DISPLAY_CHAR_Y*3);
  display.print("A:");
  display.print(lastAckMillis/1000);
  display.print(" ");
  display.print(point2point.getLastAckSNR());
  display.display();
}

void rxInd (message_t const & rxMsg)
{
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile)
  {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(rxMsg.srcAddr, HEX);
    dataFile.print(",");
    dataFile.print(rxMsg.destAddr, HEX);
    dataFile.print(",");
    dataFile.print(int(rxMsg.buf[0]));
    dataFile.print(",");
    dataFile.print(int(rxMsg.flags));
    dataFile.print(",");
    dataFile.print('1');
    dataFile.print(",");
    for (uint8_t i = 1; i < rxMsg.bufLen; i++)
    {
      dataFile.print(char(rxMsg.buf[i]));
    }
    dataFile.print(",");
    dataFile.print(spreadingFactor);
    dataFile.print(",");
    dataFile.print(signalBandwidth);
    dataFile.print(",");
    dataFile.print(frequencyChannel);
    dataFile.print(",");
    dataFile.print(txPower);
    dataFile.println();
    Serial.println("RX data written to SD card.");
  }
  else
  {
    Serial.println("SD card write failed.");
  }
  dataFile.close();
  display.fillRect(DISPLAY_RX_START, 0, DISPLAY_RX_LEN, DISPLAY_CHAR_Y*3, 0);
  display.setCursor(DISPLAY_RX_START, 0);
  display.print("From ");
  display.print(rxMsg.srcAddr, HEX);
  display.print("h");
  display.setCursor(DISPLAY_RX_START, DISPLAY_CHAR_Y);
  for (uint8_t i = 0; (i < rxMsg.bufLen) && (i < DISPLAY_RX_CHARS); i++)
  {
    display.print(char(rxMsg.buf[i]));
  }
  display.setCursor(DISPLAY_RX_START, DISPLAY_CHAR_Y*2);
  display.print("RX:");
  display.print(currentMillis/1000);
  display.display();
}

void linkChangeInd (spreadingFactor_t const newSpreadingFactor,
                    signalBandwidth_t const newSignalBandwidth,
                    frequencyChannel_t const newFrequencyChannel,
                    int8_t const newTxPower)
{
  spreadingFactor = newSpreadingFactor;
  signalBandwidth = newSignalBandwidth;
  frequencyChannel = newFrequencyChannel;
  txPower = newTxPower;
}
