/**
 * @file loraPoint2PointProtocol.cpp
 * @author Sophie Bernier (raptoronabicycle@gmail.com)
 * @brief Implementation file for the loraPoint2Point class and its dependent enums' operators, et cetera.
 * @version 0.0.1
 * @date 2021-06-29
 * 
 * @warning Under heavy development. Use at your own risk.
 * 
 */

#include <Arduino.h>
#include <loraPoint2PointProtocol.h>

//----------------------
// Function Definitions
//----------------------

void loraPoint2Point::updatePacketErrorFraction (bool packetSuccess)
{
  ++packetCount;
  packetErrorCount += !packetSuccess;
  packetErrorFraction += (float(!packetSuccess) - packetErrorFraction) /
                         MIN(packetCount, packetErrorMovingAvgPeriod);
}

void loraPoint2Point::resetPacketErrorFraction ()
{
  packetErrorFraction = 0;
  packetCount = 0;
  packetErrorCount = 0;
}

float const & loraPoint2Point::getPacketErrorFraction ()
{
  return packetErrorFraction;
}



int loraPoint2Point::getLastAckSNR ()
{
  return ackSnr;
}

spreadingFactor_t& operator++(spreadingFactor_t& s, int)
{
  switch(s)
  {
    case spreadingFactor_t::spreadingFactor_sf7:
      return s = spreadingFactor_t::spreadingFactor_sf8;
    case spreadingFactor_t::spreadingFactor_sf8:
      return s = spreadingFactor_t::spreadingFactor_sf9;
    case spreadingFactor_t::spreadingFactor_sf9:
      return s = spreadingFactor_t::spreadingFactor_sf10;
    case spreadingFactor_t::spreadingFactor_sf10:
      return s = spreadingFactor_t::spreadingFactor_sf11;
    case spreadingFactor_t::spreadingFactor_sf11:
      return s = spreadingFactor_t::spreadingFactor_sf12;
    case spreadingFactor_t::spreadingFactor_sf12:
      return s = spreadingFactor_t::spreadingFactor_sf7;
  }
}

signalBandwidth_t& operator++(signalBandwidth_t& b, int)
{
  switch(b)
  {
    case signalBandwidth_t::signalBandwidth_125kHz:
      return b = signalBandwidth_t::signalBandwidth_250kHz;
    case signalBandwidth_t::signalBandwidth_250kHz:
      return b = signalBandwidth_t::signalBandwidth_500kHz;
    case signalBandwidth_t::signalBandwidth_500kHz:
      return b = signalBandwidth_t::signalBandwidth_125kHz;
  }
}

frequencyChannel_t& operator++(frequencyChannel_t& f, int)
{
  switch(f)
  {
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_0:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_1;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_1:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_2;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_2:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_3;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_3:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_4;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_4:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_5;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_5:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_6;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_6:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_7;
    case frequencyChannel_t::frequencyChannel_500kHz_Uplink_7:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_0;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_0:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_1;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_1:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_2;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_2:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_3;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_3:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_4;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_4:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_5;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_5:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_6;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_6:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Downlink_7;
    case frequencyChannel_t::frequencyChannel_500kHz_Downlink_7:
      return f = frequencyChannel_t::frequencyChannel_500kHz_Uplink_0;      
  }
}

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

spreadingFactor_t loraPoint2Point::getSpreadingfactor ()
{
  return currentSpreadingFactor;
}

signalBandwidth_t loraPoint2Point::getSignalBandwidth ()
{
  return currentSignalBandwidth;
}

frequencyChannel_t loraPoint2Point::getFrequencyChannel ()
{
  return currentFrequencyChannel;
}

int8_t loraPoint2Point::getTxPower ()
{
  return currentTxPower;
}

void loraPoint2Point::setSpreadingFactor (spreadingFactor_t spreadingFactor)
{
  if (spreadingFactor >= NUM_spreadingFactors)
  {
    Serial.print("Invalid spreading factor setting (");
    Serial.print(spreadingFactor);
    Serial.println(")");
    return;
  }
  uint8_t spreadingFactorToSet = spreadingFactorTable[spreadingFactor];
  rf95.setSpreadingFactor(spreadingFactorToSet);
  rf95.setModeIdle(); // Required to update radio settings.
  resetPacketErrorFraction();
  previousSpreadingFactor = currentSpreadingFactor;
  currentSpreadingFactor = spreadingFactor;
  user.linkChangeInd(currentSpreadingFactor,
                     currentSignalBandwidth,
                     currentFrequencyChannel,
                     currentTxPower);
  Serial.print("Set SF to: ");
  Serial.println(spreadingFactorToSet);
}

void loraPoint2Point::setBandwidth (signalBandwidth_t bandwidth)
{
  if (bandwidth >= NUM_signalBandwidths)
  {
    Serial.print("Invalid signal bandwidth setting (");
    Serial.print(bandwidth);
    Serial.println(")");
    return;
  }
  uint32_t bandwidthToSet = signalBandwidthTable[bandwidth];
  rf95.setSignalBandwidth(bandwidthToSet);
  rf95.setModeIdle(); // Required to update radio settings.
  resetPacketErrorFraction();
  previousSignalBandwidth = currentSignalBandwidth;
  currentSignalBandwidth = bandwidth;
  user.linkChangeInd(currentSpreadingFactor,
                     currentSignalBandwidth,
                     currentFrequencyChannel,
                     currentTxPower);
  Serial.print("Set BW to: ");
  Serial.println(bandwidthToSet);
}

void loraPoint2Point::setFrequencyChannel (frequencyChannel_t frequencyChannel)
{
  if (frequencyChannel >= NUM_frequencyChannels)
  {
    Serial.print("Invalid frequency channel setting (");
    Serial.print(frequencyChannel);
    Serial.println(")");
    return;
  }
  float frequencyToSet = ((float)(frequencyChannelTable[frequencyChannel]))/10;
  if (!rf95.setFrequency(frequencyToSet))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  else
  {
    rf95.setModeIdle(); // Required to update radio settings.
    resetPacketErrorFraction();
    previousFrequencyChannel = currentFrequencyChannel;
    currentFrequencyChannel = frequencyChannel;
    user.linkChangeInd(currentSpreadingFactor,
                     currentSignalBandwidth,
                     currentFrequencyChannel,
                     currentTxPower);
    Serial.print("Set Freq to: ");
    Serial.println(frequencyToSet);
  }
}

void loraPoint2Point::setTxPower (int8_t txPower)
{
  if ((txPower > MAX_txPower) | (txPower < MIN_txPower))
  {
    Serial.print("Invalid tx power setting (");
    Serial.print(txPower);
    Serial.println("dBm)");
    return;
  }
  rf95.setTxPower(txPower, false);
  rf95.setModeIdle(); // Required to update radio settings.
  resetPacketErrorFraction();
  previousTxPower = currentTxPower;
  currentTxPower = txPower;
  user.linkChangeInd(currentSpreadingFactor,
                     currentSignalBandwidth,
                     currentFrequencyChannel,
                     currentTxPower);
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
  if (inputChar == '!')
  {
    serialCommandMode = true;
    Serial.println();
    Serial.println("Entered command mode.");
  }
  if (serialCommandMode)
  {
    if (inputChar != '\n'
        && inputChar != '\r'
        && inputChar != '!'
        && serialCmd.bufLen < RH_RF95_MAX_MESSAGE_LEN)
    {
      serialCmd.buf[serialCmd.bufLen] = inputChar;
      serialCmd.bufLen++;
    }
    if (inputChar == '\n')
    {
      if (serialCmd.bufLen >= 2)
      {
        switch (char(serialCmd.buf[0]))
        {
          case 'S':
            setSpreadingFactor(spreadingFactor_t(serialCmd.buf[1] - '0'));
            break;
          case 'B':
            setBandwidth(signalBandwidth_t(serialCmd.buf[1] - '0'));
            break;
          case 'C':
            if (serialCmd.bufLen == 2)
            {
              setFrequencyChannel(frequencyChannel_t(serialCmd.buf[1] - '0'));
            }
            else
            {
              setFrequencyChannel(frequencyChannel_t((serialCmd.buf[1] - '0') * 10 + (serialCmd.buf[2] - '0')));
            }
            break;
          case 'P':
            if (serialCmd.bufLen == 2)
            {
              setTxPower((serialCmd.buf[1] - '0'));
            }
            else
            {
              setTxPower((serialCmd.buf[1] - '0') * 10 + (serialCmd.buf[2] - '0'));
            }
            break;
          default:
            break;
        }
      }
      serialCmd.bufLen = 0;
      serialCommandMode = false;
    }
    return 0;
  }
  else
  {
    if (txMsg.bufLen == 0)
    {
      txMsg.buf[txMsg.bufLen] = msgType_dataReq;
      txMsg.bufLen++;
    }
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
}

uint8_t loraPoint2Point::setTxMessage (uint8_t const * txMsgContents, uint8_t const numChars)
{
  txMsg.buf[0] = msgType_dataReq;
  memcpy(txMsg.buf + 1, txMsgContents, numChars);
  txMsg.bufLen = numChars + 1;
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
  Serial.println("Attempting to change link to: ");
  Serial.print("SF ");
  Serial.println(spreadingFactorTable[spreadingFactor]);
  Serial.print("BW ");
  Serial.print(signalBandwidthTable[signalBandwidth]);
  Serial.println(" Hz");
  Serial.print("Channel ");
  Serial.print(float(frequencyChannelTable[frequencyChannel])/10);
  Serial.println(" MHz");
  Serial.print("TX power ");
  Serial.print(txPower);
  Serial.println(" dBm");
  if (rhReliableDatagram.sendtoWait(linkChangeReqBuf, 5, destAddress) == true)
  {
    Serial.println("Link change request acknowleged!");
    acknowleged = true;
    setSpreadingFactor(spreadingFactor);
    setBandwidth(signalBandwidth);
    setTxPower(txPower);
    setFrequencyChannel(frequencyChannel);
    // Start timer
    // call linkChangeReqTimeout if serviceLinkChangeRsp is not received in 5s.
    linkChangeTimeoutTimer.pause();
    linkChangeTimeoutTimer.clearDone();
    serviceTimers();
    linkChangeTimeoutTimer.start();
  }
  else
  {
    Serial.println("Link change request not acknowleged.");
  }
}

void loraPoint2Point::linkChangeReqTimeout ()
{
  Serial.println("Link change request timed out.");
  linkChangeTimeoutTimer.clearDone();
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
  Serial.println("Link change request received.");
  Serial.println("Attempting to change link to: ");
  Serial.print("SF ");
  Serial.println(spreadingFactorTable[spreadingFactor]);
  Serial.print("BW ");
  Serial.print(signalBandwidthTable[signalBandwidth]);
  Serial.println(" Hz");
  Serial.print("Channel ");
  Serial.print(float(frequencyChannelTable[frequencyChannel])/10);
  Serial.println(" MHz");
  Serial.print("TX power ");
  Serial.print(txPower);
  Serial.println(" dBm");
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
  delay(100); // Prevent race conditions
  if (rhReliableDatagram.sendtoWait(linkChangeRspBuf, 5, srcAddress) == true)
  {
    Serial.println("Link change response acknowleged!");
    acknowleged = true;
  }
  else
  {
    Serial.println("Link change response not acknowleged.");
    setSpreadingFactor(previousSpreadingFactor);
    setBandwidth(previousSignalBandwidth);
    setTxPower(previousTxPower);
    setFrequencyChannel(previousFrequencyChannel);
    acknowleged = false;
  }
}

void loraPoint2Point::serviceLinkChangeRsp ()
{
  Serial.println("Link change response received. Transmission OK on new settings!");
  linkChangeTimeoutTimer.pause();
  serviceTimers();
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
    for (uint8_t i = 1; i < txMsg.bufLen; i++)
    {
      Serial.print(char(txMsg.buf[i]));
    }
    Serial.println("\"");
    rf95.waitCAD();
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    if (rhReliableDatagram.sendtoWait(txMsg.buf, txMsg.bufLen, destAddress) == true)
    {
      Serial.println("Acknowleged!");
      ackSnr = rf95.lastSNR();
      Serial.print("ACK SNR: ");
      Serial.println(rf95.lastSNR());
      acknowleged = true;
    }
    updatePacketErrorFraction(acknowleged);
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
                                     200,
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
    Serial.print("RX SNR: ");
    Serial.println(rf95.lastSNR());
    switch (rxMsg.buf[0])
    {
      case msgType_dataReq:
        Serial.print("Received: \"");
        for (uint8_t i = 1; i < rxMsg.bufLen; i++)
        {
          Serial.print(char(rxMsg.buf[i]));
        }
        Serial.println("\"");
        break;
      case msgType_dataRsp:
        break;
      case msgType_linkChangeReq:
        serviceLinkChangeReq(rxMsg.srcAddr,
                             spreadingFactor_t (rxMsg.buf[1]),
                             signalBandwidth_t (rxMsg.buf[2]),
                             frequencyChannel_t(rxMsg.buf[3]),
                             int8_t            (rxMsg.buf[4]));
        break;
      case msgType_linkChangeRsp:
        serviceLinkChangeRsp ();
        break;
      default:
        break;
    }
    rxMsg.bufLen = RH_RF95_MAX_MESSAGE_LEN;
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