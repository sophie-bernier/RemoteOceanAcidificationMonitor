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
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.println("Feather LoRa Range Test - Base!");
  #endif // DEBUG_DISABLE_LOG_FUNC
  
  delay(100);  
  forceRadioReset();

  while (!rf95.init())
  {
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    #else // DEBUG_DISABLE_LOG_FUNC
    logEvent(eventType_rf95Start, eventStatus_failed, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
    while (1);
  }
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.println("LoRa radio init OK!");
  #else // DEBUG_DISABLE_LOG_FUNC
  logEvent(eventType_rf95Start, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC

  #if (USE_RH_RELIABLE_DATAGRAM > 0)
  while (!rhReliableDatagram.init())
  {
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.println("Manager init failed");
    #else // DEBUG_DISABLE_LOG_FUNC
    logEvent(eventType_reliableDatagramStart, eventStatus_failed, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
    while (1);
  }
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.println("Manager init OK!");
  #else // DEBUG_DISABLE_LOG_FUNC
  logEvent(eventType_reliableDatagramStart, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC
  #endif // USE_RH_RELIABLE_DATAGRAM

  setSpreadingFactor(spreadingFactor_sf7);
  //rf95.setSpreadingFactor(RFM95_DFLT_SF);
  setBandwidth(signalBandwidth_500kHz);
  //rf95.setSignalBandwidth(RFM95_DFLT_BW_Hz);
  rf95.setTxPower(RFM95_DFLT_TX_POWER_dBm, "");
}

void loraPoint2Point::forceRadioReset ()
{
  digitalWrite(rfm95Rst, LOW);
  delay(10);
  digitalWrite(rfm95Rst, HIGH);
  delay(10);
  #if !(DEBUG_DISABLE_LOG_FUNC > 0)
  logEvent(eventType_rf95Reset, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC
}

void loraPoint2Point::setSpreadingFactor (spreadingFactor_t spreadingFactor)
{
  if (spreadingFactor >= NUM_spreadingFactors)
  {
    spreadingFactor = spreadingFactor_sf7;
  }
  uint8_t spreadingFactorToSet = spreadingFactorTable[spreadingFactor];
  rf95.setSpreadingFactor(spreadingFactorToSet);
  eventParamStr = String(spreadingFactorToSet);
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.print("Set SF to: ");
  Serial.println(eventParamStr);
  #else // DEBUG_DISABLE_LOG_FUNC
  logEvent(eventType_setSpreadingFactor, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC
  eventParamStr = "";
}

void loraPoint2Point::setBandwidth (signalBandwidth_t bandwidth)
{
  if (bandwidth >= NUM_signalBandwidths)
  {
    bandwidth = signalBandwidth_125kHz;
  }
  uint32_t bandwidthToSet = signalBandwidthTable[bandwidth];
  eventParamStr = String(bandwidthToSet) + ", Hz";
  rf95.setSignalBandwidth(bandwidthToSet);
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.print("Set BW to: ");
  Serial.println(eventParamStr);
  #else // DEBUG_DISABLE_LOG_FUNC
  logEvent(eventType_setBandwidth, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC
  eventParamStr = "";
}

void loraPoint2Point::setFrequencyChannel (frequencyChannel_t frequencyChannel)
{
  if (frequencyChannel >= NUM_frequencyChannels)
  {
    frequencyChannel = frequencyChannel_500kHz_Uplink_0;
  }
  float frequencyToSet = ((float)(frequencyChannelTable[frequencyChannel]))/10;
  eventParamStr = String(frequencyToSet) + ", MHz";
  if (!rf95.setFrequency(frequencyToSet))
  {
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.println("setFrequency failed");
    #else // DEBUG_DISABLE_LOG_FUNC
    logEvent(eventType_setFrequency, eventStatus_failed, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
    //while (1);
  }
  else
  {
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.print("Set Freq to: ");
    Serial.println(eventParamStr);
    #else // DEBUG_DISABLE_LOG_FUNC
    logEvent(eventType_setFrequency, eventStatus_success, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
  }
  eventParamStr = "";
}

void loraPoint2Point::setTxPower (int8_t txPower)
{
  if ((txPower > 20) | (txPower < 2))
  {
    txPower = RFM95_DFLT_TX_POWER_dBm;
  }
  eventParamStr = String(txPower) + ", dBm";
  rf95.setTxPower(txPower, false);
  #if (DEBUG_DISABLE_LOG_FUNC > 0)
  Serial.print("Set TX power to: ");
  Serial.println(txPower);
  #else // DEBUG_DISABLE_LOG_FUNC
  logEvent(eventType_setTxPower, eventStatus_success, eventParamStr);
  #endif // DEBUG_DISABLE_LOG_FUNC
  eventParamStr = "";
}

uint8_t loraPoint2Point::buildStringFromSerial (Serial_* dataPort)
{
  uint8_t retval;
  char inputChar;
  while (dataPort->available())
  {
    inputChar = dataPort->read();
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.print(inputChar);
    #endif // DEBUG_DISABLE_LOG_FUNC
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
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.print(char(inputChar));
    #endif // DEBUG_DISABLE_LOG_FUNC
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
  if (txBufIdx > 0)
  {
    for (uint8_t i = 0; i < txBufIdx; i++)
    {
      eventParamStr += char(txBuf[i]);
    }
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.print("Attempting to transmit: \"");
    Serial.println(eventParamStr);
    #else // DEBUG_DISABLE_LOG_FUNC
    logEvent(eventType_messageTx, eventStatus_success, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
    eventParamStr = "";
    rf95.waitCAD();
    Serial.println(txBuf[0]);
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    if (rhReliableDatagram.sendtoWait(txBuf, txBufIdx, destAddress) == true)
    {
      #if (DEBUG_DISABLE_LOG_FUNC > 0)
      Serial.println("Sent successfully & acknowleged!");
      #else // DEBUG_DISABLE_LOG_FUNC
      logEvent(eventType_ackRx, eventStatus_success, eventParamStr);
      #endif // DEBUG_DISABLE_LOG_FUNC
    }
    txBufIdx = 0;
    #else // USE_RH_RELIABLE_DATAGRAM
    rf95.send(txBuf, txBufIdx);
    txBufIdx = 0;
    rf95.waitPacketSent();
    #endif  // USE_RH_RELIABLE_DATAGRAM
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.println("Sent successfully!");
    #endif // DEBUG_DISABLE_LOG_FUNC
  }
  else
  {
    #if (DEBUG_DISABLE_LOG_FUNC > 0)
    Serial.print("Nothing to transmit: TX buffer empty.");
    #endif // DEBUG_DISABLE_LOG_FUNC
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
    #if (USE_RH_RELIABLE_DATAGRAM > 0)
    #if !(DEBUG_DISABLE_LOG_FUNC > 0)
    logEvent(eventType_ackTx, eventStatus_success, eventParamStr);
    #endif // DEBUG_DISABLE_LOG_FUNC
    #endif  // USE_RH_RELIABLE_DATAGRAM
    for (uint8_t i = 0; i < rxBufLen; i++)
    {
      eventParamStr += char(rxBuf[i]);
    }
    switch (rxBuf[0])
    {
      default:
      #if (DEBUG_DISABLE_LOG_FUNC > 0)
      Serial.print("Received: \"");
      Serial.print(eventParamStr);
      Serial.println("\"");
      #else // DEBUG_DISABLE_LOG_FUNC
      logEvent(eventType_messageRx, eventStatus_success, eventParamStr);
      #endif // DEBUG_DISABLE_LOG_FUNC
      rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
      break;
    }
    eventParamStr = "";
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