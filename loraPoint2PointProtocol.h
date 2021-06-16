// loraPoint2PointProtocol.h

#ifndef LORA_POINT_2_POINT_PROTOCOL_H
#define LORA_POINT_2_POINT_PROTOCOL_H
#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3

#define USB_SERIAL_BAUD 115200

// The default transmitter power is 13dBm, using PA_BOOST.
// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
// you can set transmitter powers from 5 to 23 dBm:
#define RFM95_DFLT_FREQ_MHz     902.5
#define RFM95_DFLT_SF           7
#define RFM95_DFLT_TX_POWER_dBm 15
#define RFM95_DFLT_BW_Hz        500000

#define RH_RF95_MAX_MESSAGE_LEN 128

#define USE_RH_RELIABLE_DATAGRAM true

//-----------------------------------------
// Message Type and Structure Declarations
//-----------------------------------------

enum msgType_t
{
  msgType_undefined,
  msgType_dataRequest,
  msgType_linkChangeRequest,
  msgType_wakeRequest,
  msgType_sleepRequest,
  NUM_msgTypes
};

enum spreadingFactor_t
{
  spreadingFactor_sf7,
  spreadingFactor_sf8,
  spreadingFactor_sf9,
  spreadingFactor_sf10,
  spreadingFactor_sf11,
  spreadingFactor_sf12,
  NUM_spreadingFactors
};

extern uint8_t spreadingFactorTable [];

enum signalBandwidth_t
{
  signalBandwidth_125kHz,
  signalBandwidth_250kHz,
  signalBandwidth_500kHz,
  signalBandwidth_625kHz,
  NUM_signalBandwidths
};

extern uint32_t signalBandwidthTable [];

enum frequencyChannel_t
{
  frequencyChannel_500kHz_Uplink_0,
  frequencyChannel_500kHz_Uplink_1,
  frequencyChannel_500kHz_Uplink_2,
  frequencyChannel_500kHz_Uplink_3,
  frequencyChannel_500kHz_Uplink_4,
  frequencyChannel_500kHz_Uplink_5,
  frequencyChannel_500kHz_Uplink_6,
  frequencyChannel_500kHz_Uplink_7,
  frequencyChannel_500kHz_Downlink_0,
  frequencyChannel_500kHz_Downlink_1,
  frequencyChannel_500kHz_Downlink_2,
  frequencyChannel_500kHz_Downlink_3,
  frequencyChannel_500kHz_Downlink_4,
  frequencyChannel_500kHz_Downlink_5,
  frequencyChannel_500kHz_Downlink_6,
  frequencyChannel_500kHz_Downlink_7,
  NUM_frequencyChannels
};

extern uint16_t frequencyChannelTable [];

//-----------------------
// Class Declarations
//-----------------------

class loraPoint2Point
{
  public:
    loraPoint2Point (uint8_t _thisAddress,
                     uint8_t rfm95CS,
                     uint8_t rfm95Int):
                     thisAddress{_thisAddress},
                     rf95(rfm95CS, rfm95Int),
                     rhReliableDatagram(rf95, _thisAddress)
                     {}
    
    uint8_t txBuf [RH_RF95_MAX_MESSAGE_LEN];
    uint8_t txBufIdx = 0;
    uint8_t txSrcAddr = 0;
    uint8_t txDestAddr = 0;
    uint8_t txId = 0;
    uint8_t txFlags = 0;
    msgType_t txMsgType = msgType_undefined;

    uint8_t rxBuf [RH_RF95_MAX_MESSAGE_LEN];
    uint8_t rxBufLen = RH_RF95_MAX_MESSAGE_LEN;
    uint8_t rxSrcAddr = 0;
    uint8_t rxDestAddr = 0;
    uint8_t rxId = 0;
    uint8_t rxFlags = 0;
    msgType_t rxMsgType = msgType_undefined;
    
    void setupRadio ();
    void forceRadioReset ();
    void setFrequencyChannel (frequencyChannel_t frequencyChannel);
    void setSpreadingFactor  (spreadingFactor_t spreadingFactor);
    void setBandwidth        (signalBandwidth_t bandwidth); 

    uint8_t buildStringFromSerial (Serial_* dataPort);
    uint8_t buildStringFromSerial (Uart* dataPort);

    void serviceTx (uint8_t destAddress);
    void serviceRx ();
    
    //sendLinkChangeRequest (uint8_t destAddr;
    //                       spreadingFactor_t spreadingFactor,
    //                       signalBandwidth_t signalBandwidth,
    //                       int8_t txPower);
    //checkForMessages ();
    //sendSleepRequest (uint8_t destAddr);
    //sendWakeRequest (uint8_t destAddr);
    //goToSleep ();
  private:
    //uint8_t (*serviceMsgData)()
    //uint8_t (*serviceMsgLinkAdapt)()
    uint8_t thisAddress;
    uint8_t buildStringFromSerialInner (char inputChar);
    
    const uint8_t spreadingFactorTable [NUM_spreadingFactors] = {7, 8, 9, 10, 11, 12};
    const uint32_t signalBandwidthTable [NUM_signalBandwidths] = {125000, 250000, 500000, 625000};
    const uint16_t frequencyChannelTable [NUM_frequencyChannels] = {9030, 9046, 9062, 9078, 9094, 9110, 9126, 9142, 9233, 9239, 9245, 9251, 9257, 9263, 9269, 9275};
    
    RH_RF95 rf95;
    RHReliableDatagram rhReliableDatagram;
    
};


//-----------------------
// Function Declarations
//-----------------------

#endif // LORA_POINT_2_POINT_PROTOCOL_H