// loraPoint2PointProtocol.cpp

#include <Arduino.h>
#include <loraPoint2PointProtocol.h>

//----------------------
// Function Definitions
//----------------------

void loraPoint2Point::setupRadio ()
{
  pinMode(rfm95Rst, OUTPUT);
  digitalWrite(rfm95Rst, HIGH);
  Serial.println("Feather LoRa Range Test");
  delay(100);  
  forceRadioReset();

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  #if (USE_RH_RELIABLE_DATAGRAM > 0)
  while (!rhReliableDatagram.init())
  {
    Serial.println("Manager init failed");
    while (1);
  }
  Serial.println("Manager init OK!");
  #endif // USE_RH_RELIABLE_DATAGRAM

  setSpreadingFactor(spreadingFactor_sf7);
  setBandwidth(signalBandwidth_500kHz);
  rf95.setTxPower(RFM95_DFLT_TX_POWER_dBm, false);
}

void loraPoint2Point::forceRadioReset ()
{
  digitalWrite(rfm95Rst, LOW);
  delay(10);
  digitalWrite(rfm95Rst, HIGH);
  delay(10);
}

void loraPoint2Point::setSpreadingFactor (spreadingFactor_t spreadingFactor)
{
  if (spreadingFactor >= NUM_spreadingFactors)
  {
    spreadingFactor = spreadingFactor_sf7;
  }
  uint8_t spreadingFactorToSet = spreadingFactorTable[spreadingFactor];
  rf95.setSpreadingFactor(spreadingFactorToSet);
  Serial.print("Set SF to: ");
  Serial.println(spreadingFactorToSet);
}

void loraPoint2Point::setBandwidth (signalBandwidth_t bandwidth)
{
  if (bandwidth >= NUM_signalBandwidths)
  {
    bandwidth = signalBandwidth_125kHz;
  }
  uint32_t bandwidthToSet = signalBandwidthTable[bandwidth];
  rf95.setSignalBandwidth(bandwidthToSet);
  Serial.print("Set BW to: ");
  Serial.println(bandwidthToSet);
}

void loraPoint2Point::setFrequencyChannel (frequencyChannel_t frequencyChannel)
{
  if (frequencyChannel >= NUM_frequencyChannels)
  {
    frequencyChannel = frequencyChannel_500kHz_Uplink_0;
  }
  float frequencyToSet = ((float)(frequencyChannelTable[frequencyChannel]))/10;
  if (!rf95.setFrequency(frequencyToSet))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  else
  {
    Serial.print("Set Freq to: ");
    Serial.println(frequencyToSet);
  }
}

void loraPoint2Point::setTxPower (int8_t txPower)
{
  if ((txPower > 20) | (txPower < 2))
  {
    txPower = RFM95_DFLT_TX_POWER_dBm;
  }
  rf95.setTxPower(txPower, false);
  Serial.print("Set TX power to: ");
  Serial.println(txPower);
}

uint8_t loraPoint2Point::buildStringFromSerial (Serial_* dataPort)
{
  uint8_t retval;
  char inputChar;
  while (dataPort->available())
  {
    inputChar = dataPort->read();
    Serial.print(char(inputChar));
    retval = buildStringFromSerialInner(inputChar);
  }
  return retval;
}

uint8_t loraPoint2Point::buildStringFromSerial (Uart* dataPort)
{
  uint8_t retval;
  char inputChar;
  while (dataPort->available())
  {
    inputChar = dataPort->read();
    Serial.print(char(inputChar));
    retval = buildStringFromSerialInner(inputChar);
  }
  return retval;
}

uint8_t loraPoint2Point::buildStringFromSerialInner (char inputChar)
{
  // Add chars to ignore to this.
  if ((inputChar != '\n'
       && inputChar != '\r')
      && txBufIdx < RH_RF95_MAX_MESSAGE_LEN)
  {
    txBuf[txBufIdx] = inputChar;
    txBufIdx++;
  }
  // Special start key
  if (inputChar == '$')
  {
    txBuf[txBufIdx] = 27;
    txBufIdx++;
  }
  // \n triggers sending
  if (inputChar == '\n')
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void loraPoint2Point::serviceTx (uint8_t destAddress)
{
  bool acknowleged = false;
  if (txBufIdx > 0)
  {
    Serial.print("Attempting to transmit: \"");
    for (uint8_t i = 0; i < txBufIdx; i++)
    {
      Serial.print(char(txBuf[i]));
    }
    Serial.println("\"");
    rf95.waitCAD();
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    if (rhReliableDatagram.sendtoWait(txBuf, txBufIdx, destAddress) == true)
    {
      Serial.println("Acknowleged!");
      acknowleged = true;
    }
    if (acknowleged == false)
    {
      Serial.println("Not acknowleged.");
    }
    txBufIdx = 0;
    #else // USE_RH_RELIABLE_DATAGRAM
    rf95.send(txBuf, txBufIdx);
    txBufIdx = 0;
    rf95.waitPacketSent();
    Serial.println("Sent successfully!");
    #endif  // USE_RH_RELIABLE_DATAGRAM
  }
  else
  {
    Serial.print("Nothing to transmit: TX buffer empty.");
  }
}

void loraPoint2Point::serviceRx ()
{
  if (
      #if (USE_RH_RELIABLE_DATAGRAM > 0)
      rhReliableDatagram.recvfromAckTimeout(rxBuf,
                                     &rxBufLen,
                                     500,
                                     &rxSrcAddr,
                                     &rxDestAddr,
                                     &rxId,
                                     &rxFlags)
      #else // USE_RH_RELIABLE_DATAGRAM
      rf95.recv(rxBuf, &rxBufLen)
      #endif  // USE_RH_RELIABLE_DATAGRAM
     )
  {
    switch (rxBuf[0])
    {
      default:
      Serial.print("Received: \"");
      for (uint8_t i = 0; i < rxBufLen; i++)
      {
        Serial.print(char(rxBuf[i]));
      }
      Serial.println("\"");
      rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
      break;
    }
  }
}

/*
uint8_t serviceMsg (uint8_t* buf,
                    uint8_t* len,
                    uint8_t srcAddr,
                    uint8_t destAddr,
                    uint8_t id,
                    uint8_t flags);
*/