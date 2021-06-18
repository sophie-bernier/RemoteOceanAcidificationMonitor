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
      && txMsg.bufLen < RH_RF95_MAX_MESSAGE_LEN)
  {
    txMsg.buf[txMsg.bufLen] = inputChar;
    txMsg.bufLen++;
  }
  // Special start key
  if (inputChar == '$')
  {
    txMsg.buf[txMsg.bufLen] = 27;
    txMsg.bufLen++;
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

// Todo: smarter way of doing this.
uint8_t loraPoint2Point::setTxMessage (uint8_t const * txMsgContents, uint8_t const numChars)
{
  memcpy(txMsg.buf, txMsgContents, numChars);
  txMsg.bufLen = numChars;
  return numChars;
}

void loraPoint2Point::serviceTx (uint8_t destAddress)
{
  bool acknowleged = false;
  if (txMsg.bufLen > 0)
  {
    Serial.print("Attempting to transmit: \"");
    for (uint8_t i = 0; i < txMsg.bufLen; i++)
    {
      Serial.print(char(txMsg.buf[i]));
    }
    Serial.println("\"");
    rf95.waitCAD();
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    if (rhReliableDatagram.sendtoWait(txMsg.buf, txMsg.bufLen, destAddress) == true)
    {
      Serial.println("Acknowleged!");
      acknowleged = true;
    }
    if (acknowleged == false)
    {
      Serial.println("Not acknowleged.");
    }
    #else // USE_RH_RELIABLE_DATAGRAM
    rf95.send(txMsg.buf, txMsg.bufLen);
    rf95.waitPacketSent();
    Serial.println("Sent successfully!");
    #endif  // USE_RH_RELIABLE_DATAGRAM
    user.txInd(txMsg.buf, txMsg.bufLen, destAddress, acknowleged);
    txMsg.bufLen = 0;
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
      rhReliableDatagram.recvfromAckTimeout(rxMsg.buf,
                                     &rxMsg.bufLen,
                                     500,
                                     &rxMsg.srcAddr,
                                     &rxMsg.destAddr,
                                     &rxMsg.msgId,
                                     &rxMsg.flags)
      #else // USE_RH_RELIABLE_DATAGRAM
      rf95.recv(rxMsg.buf, &(rxMsg.bufLen))
      #endif  // USE_RH_RELIABLE_DATAGRAM
     )
  {
    user.rxInd(rxMsg);
    switch (rxMsg.buf[0])
    {
      default:
      Serial.print("Received: \"");
      for (uint8_t i = 0; i < rxMsg.bufLen; i++)
      {
        Serial.print(char(rxMsg.buf[i]));
      }
      Serial.println("\"");
      rxMsg.bufLen = RH_RF95_MAX_MESSAGE_LEN;
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