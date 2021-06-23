// loraPoint2PointProtocol.cpp

#include <Arduino.h>
#include <loraPoint2PointProtocol.h>

//----------------------
// Function Definitions
//----------------------

bool loraPoint2Point::setupRadio ()
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
    return false;
  }
  Serial.println("LoRa radio init OK!");

  #if (USE_RH_RELIABLE_DATAGRAM > 0)
  while (!rhReliableDatagram.init())
  {
    Serial.println("Manager init failed");
    return false;
  }
  Serial.println("Manager init OK!");
  #endif // USE_RH_RELIABLE_DATAGRAM

  setSpreadingFactor(RFM95_DFLT_SPREADING_FACTOR);
  setBandwidth(RFM95_DFLT_SIGNAL_BANDWIDTH);
  setTxPower(RFM95_DFLT_TX_POWER_dBm);
  setFrequencyChannel(RFM95_DFLT_FREQ_CHANNEL);
  return true;
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
    spreadingFactor = RFM95_DFLT_SPREADING_FACTOR;
  }
  uint8_t spreadingFactorToSet = spreadingFactorTable[spreadingFactor];
  rf95.setSpreadingFactor(spreadingFactorToSet);
  previousSpreadingFactor = currentSpreadingFactor;
  currentSpreadingFactor = spreadingFactor;
  Serial.print("Set SF to: ");
  Serial.println(spreadingFactorToSet);
}

void loraPoint2Point::setBandwidth (signalBandwidth_t bandwidth)
{
  if (bandwidth >= NUM_signalBandwidths)
  {
    bandwidth = RFM95_DFLT_SIGNAL_BANDWIDTH;
  }
  uint32_t bandwidthToSet = signalBandwidthTable[bandwidth];
  rf95.setSignalBandwidth(bandwidthToSet);
  previousSignalBandwidth = currentSignalBandwidth;
  currentSignalBandwidth = bandwidth;
  Serial.print("Set BW to: ");
  Serial.println(bandwidthToSet);
}

void loraPoint2Point::setFrequencyChannel (frequencyChannel_t frequencyChannel)
{
  if (frequencyChannel >= NUM_frequencyChannels)
  {
    frequencyChannel = RFM95_DFLT_FREQ_CHANNEL;
  }
  float frequencyToSet = ((float)(frequencyChannelTable[frequencyChannel]))/10;
  if (!rf95.setFrequency(frequencyToSet))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  else
  {
    previousFrequencyChannel = currentFrequencyChannel;
    currentFrequencyChannel = frequencyChannel;
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
  previousTxPower = currentTxPower;
  currentTxPower = txPower;
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

void loraPoint2Point::linkChangeReq (uint8_t const            destAddress,
                                     spreadingFactor_t const  spreadingFactor,
                                     signalBandwidth_t const  signalBandwidth,
                                     frequencyChannel_t const frequencyChannel,
                                     int8_t const             txPower)
{
  bool acknowleged = false;
  uint8_t linkChangeReqBuf [] = {msgType_linkChangeReq,
                                 uint8_t(spreadingFactor),
                                 uint8_t(signalBandwidth),
                                 uint8_t(frequencyChannel),
                                 uint8_t(txPower)};
  if (rhReliableDatagram.sendtoWait(linkChangeReqBuf, 5, destAddress) == true)
  {
    Serial.println("Acknowleged!");
    acknowleged = true;
    setSpreadingFactor(spreadingFactor);
    setBandwidth(signalBandwidth);
    setTxPower(txPower);
    setFrequencyChannel(frequencyChannel);
    // Start timer
    // call linkChangeReqTimeout if serviceLinkChangeRsp is not received in 5s.
    linkChangeTimeoutTimer.reset();
    linkChangeTimeoutTimer.start();
  }
}

void loraPoint2Point::linkChangeReqTimeout ()
{
  setSpreadingFactor(previousSpreadingFactor);
  setBandwidth(previousSignalBandwidth);
  setTxPower(previousTxPower);
  setFrequencyChannel(previousFrequencyChannel);
}

void loraPoint2Point::serviceLinkChangeReq (uint8_t const            srcAddress,
                                            spreadingFactor_t const  spreadingFactor,
                                            signalBandwidth_t const  signalBandwidth,
                                            frequencyChannel_t const frequencyChannel,
                                            int8_t const             txPower)
{
  bool acknowleged = false;
  setSpreadingFactor(spreadingFactor);
  setBandwidth(signalBandwidth);
  setTxPower(txPower);
  setFrequencyChannel(frequencyChannel);
  uint8_t linkChangeRspBuf [] = {msgType_linkChangeRsp,
                                 uint8_t(currentSpreadingFactor),
                                 uint8_t(currentSignalBandwidth),
                                 uint8_t(currentFrequencyChannel),
                                 uint8_t(currentTxPower)};
  if (rhReliableDatagram.sendtoWait(linkChangeRspBuf, 5, srcAddress) == true)
  {
    Serial.println("Acknowleged!");
    acknowleged = true;
  }
  else
  {
    setSpreadingFactor(previousSpreadingFactor);
    setBandwidth(previousSignalBandwidth);
    setTxPower(previousTxPower);
    setFrequencyChannel(previousFrequencyChannel);
    acknowleged = false;
  }
}

void loraPoint2Point::serviceLinkChangeRsp ()
{
  linkChangeTimeoutTimer.pause();
  linkChangeTimeoutTimer.reset();
}

void loraPoint2Point::serviceTimers ()
{
  currentMillis = millis();
  linkChangeTimeoutTimer.update();
  if (linkChangeTimeoutTimer.isDone())
  {
    linkChangeReqTimeout();
    linkChangeTimeoutTimer.clearDone();
  }
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
    Serial.println("Nothing to transmit: TX buffer empty.");
  }
}

void loraPoint2Point::serviceRx ()
{
  serviceTimers();
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