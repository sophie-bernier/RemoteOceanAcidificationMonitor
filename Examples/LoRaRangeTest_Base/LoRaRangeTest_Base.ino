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


//---------
// Classes
//---------

loraPoint2Point point2point(RH_RELIABLE_DATAGRAM_ADDR,
                            RFM95_CS,
                            RFM95_INT,
                            RFM95_RST);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
File dataFile;
String dataFileName = "datalog.txt";
String dataFileHeader = "Event, Action, Status, NumParams, Params";

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
  }
  
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

void loop()
{
  // Transmit a string!
  if (point2point.buildStringFromSerial(&Serial))
  {
    point2point.serviceTx(0xEE);
  }
  point2point.serviceRx();
}

//-------------------------------
// Callback function definitions
//-------------------------------
