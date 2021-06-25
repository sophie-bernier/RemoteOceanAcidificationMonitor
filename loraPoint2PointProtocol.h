// loraPoint2PointProtocol.h

#ifndef LORA_POINT_2_POINT_PROTOCOL_H
#define LORA_POINT_2_POINT_PROTOCOL_H
#include <Arduino.h>
//#include <list.h>
#include <simpleTimer.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

// The default transmitter power is 13dBm, using PA_BOOST.
// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
// you can set transmitter powers from 5 to 23 dBm:
#define RFM95_DFLT_FREQ_CHANNEL     frequencyChannel_500kHz_Uplink_0
#define RFM95_DFLT_SPREADING_FACTOR spreadingFactor_sf7 // spreadingFactor_sf8
#define RFM95_DFLT_TX_POWER_dBm     15
#define RFM95_DFLT_SIGNAL_BANDWIDTH signalBandwidth_500kHz // signalBandwidth_250kHz
#define RH_RF95_MAX_MESSAGE_LEN 128

#define LINK_CHANGE_TIMEOUT_MILLIS 3000

#define USE_RH_RELIABLE_DATAGRAM true

//-----------------------------------------
// Message Type and Structure Declarations
//-----------------------------------------

struct message_t
{
  uint8_t srcAddr;
  uint8_t destAddr;
  uint8_t msgId;
  uint8_t flags;
  uint8_t bufLen;
  uint8_t buf [RH_RF95_MAX_MESSAGE_LEN];
};

struct userCallbacks_t
{
  void (*txInd) (uint8_t const * txBuf, 
                 uint8_t const bufLen, 
                 uint8_t const destAddr, 
                 bool ack);
  void (*rxInd) (message_t const & rxMsg);
};

enum eventType_t
{
  eventType_rf95Reset,
  eventType_rf95Start,
  eventType_reliableDatagramStart,
  eventType_setSpreadingFactor,
  eventType_setBandwidth,
  eventType_setFrequency,
  eventType_setTxPower,
  eventType_messageTx,
  eventType_messageRx,
  eventType_ackTx,
  eventType_ackRx,
  NUM_events
};

enum eventStatus_t
{
  eventStatus_failed,
  eventStatus_success,
  NUM_eventStatuses
};

enum msgType_t
{
  msgType_undefined,
  msgType_dataReq,
  msgType_dataRsp,
  msgType_linkChangeReq,
  msgType_linkChangeRsp,
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

spreadingFactor_t& operator++(spreadingFactor_t& s, int);

enum signalBandwidth_t
{
  signalBandwidth_125kHz,
  signalBandwidth_250kHz,
  signalBandwidth_500kHz,
  signalBandwidth_625kHz,
  NUM_signalBandwidths
};

signalBandwidth_t& operator++(signalBandwidth_t& b, int);
 
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

frequencyChannel_t& operator++(frequencyChannel_t& f, int);

#define MAX_txPower 20

//-----------------------
// Class Declarations
//-----------------------

class loraPoint2Point
{
  public:
    // Constructor
    loraPoint2Point (uint8_t const _thisAddress,
                     uint8_t const rfm95CS,
                     uint8_t const rfm95Int,
                     uint8_t const rfm95Rst,
                     userCallbacks_t const & userCallbacks
                     ):
                       thisAddress{_thisAddress},
                       user{userCallbacks},
                       rf95(rfm95CS, rfm95Int),
                       rhReliableDatagram(rf95, _thisAddress)
                       {

                       }
    
    // Public functions
    bool setupRadio ();
    void setFrequencyChannel (frequencyChannel_t frequencyChannel);
    void setSpreadingFactor  (spreadingFactor_t spreadingFactor);
    void setBandwidth        (signalBandwidth_t bandwidth); 
    void setTxPower          (int8_t txPower);
    uint8_t setTxMessage          (uint8_t const * txMsgContents,
                                   uint8_t const numChars);
    uint8_t buildStringFromSerial (Serial_* dataPort);
    uint8_t buildStringFromSerial (Uart* dataPort);
    void serviceTx (uint8_t destAddress);
    void serviceRx ();
    /**
     * Client       Server   Client            Server
     * (Base)      (Sensor)  (Base)           (Sensor)
     *  |                |    |                     |
     *  |--Request(Req)->|    |<-Indication(Ind)----|
     *  |<-Response(Rsp)-|    |--Confirmation(Cnf)->|
     *  V                V    V                     V
     */
    void dataReq        (); // Client only
    void linkChangeReq  (uint8_t const            destAddress,
                         spreadingFactor_t const  spreadingFactor,
                         signalBandwidth_t const  signalBandwidth,
                         frequencyChannel_t const frequencyChannel,
                         int8_t const             txPower);
    void serviceTimers (); // Call this regularly in the main loop
    
    //sendLinkChangeRequest (uint8_t destAddr;
    //                       spreadingFactor_t spreadingFactor,
    //                       signalBandwidth_t signalBandwidth,
    //                       int8_t txPower);
    //checkForMessages ();
    //sendSleepRequest (uint8_t destAddr);
    //sendWakeRequest (uint8_t destAddr);
    //goToSleep ();
  private:
    // Private variables
    bool serialCommandMode = false;
    uint8_t rfm95Rst = 4;
    uint8_t thisAddress = 0;
    spreadingFactor_t  currentSpreadingFactor  = RFM95_DFLT_SPREADING_FACTOR;
    signalBandwidth_t  currentSignalBandwidth  = RFM95_DFLT_SIGNAL_BANDWIDTH;
    frequencyChannel_t currentFrequencyChannel = RFM95_DFLT_FREQ_CHANNEL;
    int8_t             currentTxPower          = RFM95_DFLT_TX_POWER_dBm;
    spreadingFactor_t  previousSpreadingFactor  = currentSpreadingFactor;
    signalBandwidth_t  previousSignalBandwidth  = currentSignalBandwidth;
    frequencyChannel_t previousFrequencyChannel = currentFrequencyChannel;
    int8_t             previousTxPower          = currentTxPower;
    message_t txMsg = {0, 0, 0, 0, 0};
    message_t rxMsg = {0, 0, 0, 0, RH_RF95_MAX_MESSAGE_LEN};
    message_t serialCmd = {0, 0, 0, 0, 0};
    const uint8_t spreadingFactorTable [NUM_spreadingFactors] = {7, 8, 9, 10, 11, 12};
    const uint32_t signalBandwidthTable [NUM_signalBandwidths] = {125000, 250000, 500000, 625000};
    const uint16_t frequencyChannelTable [NUM_frequencyChannels] = {9030, 9046, 9062, 9078, 9094, 9110, 9126, 9142, 9233, 9239, 9245, 9251, 9257, 9263, 9269, 9275};
    uint32_t currentMillis = 0;
    
    // Private classes
    RH_RF95 rf95;
    RHReliableDatagram rhReliableDatagram;
    simpleTimer linkChangeTimeoutTimer = simpleTimer(LINK_CHANGE_TIMEOUT_MILLIS,
                                                     currentMillis,
                                                     false);
    //list<simpleTimer*> simpleTimerList;
    
    // Private functions
    uint8_t buildStringFromSerialInner (char inputChar);
    void forceRadioReset ();
    void serviceDataReq (); // Server only
    void serviceDataRsp (); // Client only
    void serviceLinkChangeReq (uint8_t const            srcAddress,
                               spreadingFactor_t const  spreadingFactor,
                               signalBandwidth_t const  signalBandwidth,
                               frequencyChannel_t const frequencyChannel,
                               int8_t const             txPower);
    void serviceLinkChangeRsp ();
    void linkChangeReqTimeout ();

    
    // Callback functions
    userCallbacks_t user;
};


//-----------------------
// Function Declarations
//-----------------------

#endif // LORA_POINT_2_POINT_PROTOCOL_H