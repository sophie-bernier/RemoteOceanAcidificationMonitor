// Echoes serial communication over UART and vice versa.
// Koichi approved.
// Designed to work with the RFM95 Feather M0 board.
// This is the client, with SD logging and an OLED display.

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <loraPoint2PointProtocol.h>

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3
#define RH_RELIABLE_DATAGRAM_ADDR 0xBB

loraPoint2Point point2point(RH_RELIABLE_DATAGRAM_ADDR, RFM95_CS, RFM95_INT);

//------------
// Main Setup
//------------

void setup()
{
  digitalWrite(10, HIGH); // tie SD high
  point2point.setupRadio();
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
