// loraPoint2PointProtocol.h

#ifndef LORA_POINT_2_POINT_PROTOCOL_H
#define LORA_POINT_2_POINT_PROTOCOL_H
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

enum msgType_t
{
  msgType_dataRequest,
  msgType_linkChangeRequest,
  msgType_wakeRequest,
  msgType_sleepRequest
};

enum spreadingFactor_t
{
  spreadingFactor_sf7,
  spreadingFactor_sf8,
  spreadingFactor_sf9,
  spreadingFactor_sf10,
  spreadingFactor_sf11,
  spreadingFactor_sf12
};

enum signalBandwidth_t
{
  signalBandwidth_125kHz,
  signalBandwidth_250kHz,
  signalBandwidth_500kHz,
  signalBandwidth_625kHz
};

class loraPoint2Point
{
  public;
    loraPoint2Point (uint8_t thisAddress,
                     uint8_t (*_serviceMsgData)(uint8_t* buf, 
                                                uint8_t* len,
                                                uint8_t srcAddr,
                                                uint8_t destAddr,
                                                uint8_t id,
                                                uint8_t flags),
                     uint8_t (*_serviceAck)(uint8_t srcAddr));
    sendLinkChangeRequest (uint8_t destAddr;
                           spreadingFactor_t spreadingFactor,
                           signalBandwidth_t signalBandwidth,
                           int8_t txPower);
    checkForMessages ();
    sendSleepRequest (uint8_t destAddr);
    sendWakeRequest (uint8_t destAddr);
    goToSleep ();
  private;
    uint8_t (*serviceMsgData)()
    uint8_t (*serviceMsgLinkAdapt)()
    RH_RF95 rf95;
};

uint8_t serviceMsg (uint8_t* buf,
                    uint8_t* len,
                    uint8_t srcAddr,
                    uint8_t destAddr,
                    uint8_t id,
                    uint8_t flags);

#endif // LORA_POINT_2_POINT_PROTOCOL_H