// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.
// This is the client, with SD logging and an OLED display.

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

#define SD_CS     10
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RH_RELIABLE_DATAGRAM_ADDR 0xBB

#define USB_SERIAL_BAUD 115200

//--------------------------------
// Callback function declarations
//--------------------------------

void txInd (uint8_t const * txBuf,
            uint8_t const bufLen,
            uint8_t const destAddr,
            bool ack);
void rxInd (message_t const & rxMsg);

//---------
// Classes
//---------

userCallbacks_t callbacks = {txInd, rxInd};

loraPoint2Point point2point(RH_RELIABLE_DATAGRAM_ADDR,
                            RFM95_CS,
                            RFM95_INT,
                            RFM95_RST,
                            callbacks);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
File dataFile;
String dataFileName = "datalog.txt";
String dataFileHeader = "Timestamp,Source Address,Destination Address,Message ID,Message Flags,Acknowleged,Message";

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
  
  digitalWrite(10, HIGH); // tie SD high
  point2point.setupRadio();

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS))
  {
    Serial.println("Card failed or not present");
  }
  else
  {
    Serial.println("Card initialized.");
  }
  
  dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(dataFileHeader);
    Serial.println("Header written to SD card.");
  }
  else
  {
    Serial.println("SD card write failed.");
  }
  dataFile.close();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Display initialization failed.");
  }
  else
  {
    Serial.println("Display initialized.");
  }
  display.display();
}

//-----------
// Main Loop
//-----------

uint32_t prevMillis = 0;
uint32_t currentMillis = 0;
bool timeUp = false;

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
  
  if ((currentMillis - prevMillis) > 5000)
  {
    Serial.println("foo");
    point2point.setTxMessage((uint8_t*)("foobar"), 6);
    prevMillis = currentMillis;
    timeUp = true;
  }
  
  if (point2point.buildStringFromSerial(&Serial) || timeUp)
  {
    point2point.serviceTx(0xEE);
    Serial.println("bar");
    timeUp = false;
  }
  
  point2point.serviceRx();
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
    dataFile.print(",,,");
    dataFile.print(ack);
    dataFile.print(",");
    for (uint8_t i = 0; i < bufLen; i++)
    {
      dataFile.print(char(txBuf[i]));
    }
    dataFile.println();
    Serial.println("TX data written to SD card.");
  }
  else
  {
    Serial.println("SD card write failed.");
  }
  dataFile.close();
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
    dataFile.print(rxMsg.msgId);
    dataFile.print(",");
    dataFile.print(rxMsg.flags);
    dataFile.print(",,");
    for (uint8_t i = 0; i < rxMsg.bufLen; i++)
    {
      dataFile.print(char(rxMsg.buf[i]));
    }
    dataFile.println();
    Serial.println("RX data written to SD card.");
  }
  else
  {
    Serial.println("SD card write failed.");
  }
  dataFile.close();
}
