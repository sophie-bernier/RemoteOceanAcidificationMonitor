// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.
// This is the server.

// foobarbaz

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
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
void rxInd (message_t const & rxMsg)
{}
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
  
  digitalWrite(10, HIGH); // tie SD high
  point2point.setupRadio();
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

void loop()
{
  currentMillis = millis();
  
  // Transmit a string!
  if (point2point.buildStringFromSerial(&Serial))
  {
    point2point.serviceTx(0xBB);
  }

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
  point2point.serviceRx();
}

//-------------------------------
// Callback function definitions
//-------------------------------
