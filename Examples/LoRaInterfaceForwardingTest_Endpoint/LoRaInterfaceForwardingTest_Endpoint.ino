// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.
// This is the server.

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RHDatagram.h>
#include <loraPoint2PointProtocol.h>

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RH_RELIABLE_DATAGRAM_ADDR 0xEE

#define USB_SERIAL_BAUD 115200

//--------------------------------
// Callback function declarations
//--------------------------------

void txInd (uint8_t const * txBuf,
            uint8_t const bufLen,
            uint8_t const destAddr,
            bool ack)
{}

void rxInd (message_t const & rxMsg);

void linkChangeInd (spreadingFactor_t const newSpreadingFactor,
                    signalBandwidth_t const newSignalBandwidth,
                    frequencyChannel_t const newFrequencyChannel,
                    int8_t const newTxPower)
{}


//---------
// Classes
//---------

userCallbacks_t callbacks = {txInd, rxInd, linkChangeInd};

loraPoint2Point point2point(RH_RELIABLE_DATAGRAM_ADDR,
                            RFM95_CS,
                            RFM95_INT,
                            RFM95_RST,
                            callbacks);

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
  Serial.println("Feather LoRa Sensor Interface Forwarding Test - Endpoint");
  Serial1.begin(9600);
  while (!Serial1)
  {
    delay(1);
  }
  Serial.println("HW Serial 1 init OK!");
  digitalWrite(10, HIGH); // tie SD high
  point2point.setupRadio();
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  digitalWrite(14, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);
  digitalWrite(15, LOW);
  digitalWrite(16, HIGH);
  digitalWrite(16, LOW);
  digitalWrite(17, HIGH);
  digitalWrite(17, LOW);
}
 
//-----------
// Main Loop
//-----------

uint32_t prevMillis = 0;
uint32_t currentMillis = 0;
uint32_t lastAckMillis = 0;
uint32_t prevLinkChangeMillis = 0;
bool timeUp = false;
uint8_t varToIncrement = 0;

String txString = String();
#define TX_BUF_LEN 255
#define DATA_FIELD_LEN TX_BUF_LEN - 1
uint8_t txBuf [TX_BUF_LEN];
uint8_t txStringLen = 0;
uint8_t* dataField = txBuf + 1;
uint8_t newChar;
bool reading = false;
bool done = true;

// 14-19 are good
void loop()
{
  currentMillis = millis();
  
  // Transmit a string!
  /* 
  if (point2point.buildStringFromSerial(&Serial1))
  {
    point2point.serviceTx(0xBB);
  }
  */
   
  //txString = Serial1.readStringUntil('\r');

  if (Serial1.available())
  {
    done = false;
  }
  digitalWrite(14, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(14, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(14, HIGH);
  while(done == false && Serial1.available())
  {
    newChar = Serial1.read();
    switch (newChar)
    {
      case '\n': // terminate and exit
      case '\r':
      case '>':
      case '*':
        done = true;
      case ' ': // ignore
        break;
      default:
        Serial.print(char(newChar));
        txBuf[txStringLen++] = newChar;
    }
  }
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);
  digitalWrite(15, LOW);
  digitalWrite(15, HIGH);
  digitalWrite(15, LOW);
  digitalWrite(15, HIGH);
  if (txStringLen > 1
      && done == true)
  {
    txBuf[0] = msgType_dataReq;
    Serial.println(" TX");
    point2point.rf95.waitCAD();
    //point2point.rhDatagram.sendto(txBuf, txStringLen, 0xBB);
    digitalWrite(17, HIGH);
    digitalWrite(17, LOW);
    digitalWrite(17, HIGH);
    digitalWrite(17, LOW);
    digitalWrite(17, HIGH);
    point2point.rhReliableDatagram.sendtoWait(txBuf, txStringLen, 0xBB);
    digitalWrite(17, LOW);
    txStringLen = 1;
    //txString.getBytes(dataField, DATA_FIELD_LEN);
    //Serial.println(txString);
    //point2point.rhDatagram.sendto(txBuf, txString.length() + 1, 0xBB);
  }
  digitalWrite(15, LOW);
  

  /*  
  if 
     //(false)
     ((currentMillis - prevLinkChangeMillis) > 21010)
  {
    //point2point.linkChangeReq(0xEE, spreadingFactor, signalBandwidth, frequencyChannel, txPower);
    prevLinkChangeMillis = currentMillis;

    switch (varToIncrement)
    {
    case 0:
      //spreadingFactor = spreadingFactor_sf8;
      point2point.setSpreadingFactor(spreadingFactor_sf8);
      varToIncrement++;
      break;
    case 1:
      //signalBandwidth = signalBandwidth_250kHz;
      point2point.setBandwidth(signalBandwidth_250kHz);
      varToIncrement++;
      break;
    case 2:
      //frequencyChannel = frequencyChannel_500kHz_Uplink_1;
      point2point.setFrequencyChannel(frequencyChannel_500kHz_Uplink_1);
      varToIncrement++;
      break;
    case 3:
      //txPower++;
      point2point.setTxPower(16);
      varToIncrement++;
      break;
    default:
      break;
    }
  }
  */
  digitalWrite(16, HIGH);
  digitalWrite(16, LOW);
  digitalWrite(16, HIGH);
  digitalWrite(16, LOW);
  digitalWrite(16, HIGH);
  if (!Serial1.available() && done == true)
  {
    point2point.serviceRx();
  }
  digitalWrite(16, LOW);
}

//-------------------------------
// Callback function definitions
//-------------------------------

void rxInd (message_t const & rxMsg)
{
  if (rxMsg.buf[0] == msgType_dataReq)
  {
    for (uint8_t i = 1; i < rxMsg.bufLen; i++)
    {
      Serial1.print(char(rxMsg.buf[i]));
    }
  }
}
