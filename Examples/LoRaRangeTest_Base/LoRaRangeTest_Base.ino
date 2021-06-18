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

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Display initialization failed.");
  }
  else
  {
    Serial.println("Display initialized.");
  }
  
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
}

//-----------
// Main Loop
//-----------

uint32_t prevMillis = 0;
uint32_t currentMillis = 0;
uint32_t lastAckMillis = 0;
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
  display.print("A.:");
  display.print(lastAckMillis/1000);
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
  display.fillRect(DISPLAY_RX_START, 0, DISPLAY_RX_LEN, DISPLAY_Y, 0);
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
