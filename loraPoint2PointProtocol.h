/**
 * @file loraPoint2PointProtocol.h
 * @author Sophie Bernier (raptoronabicycle@gmail.com)
 * @brief Header file for the loraPoint2Point class and its dependent enums, defaults, et cetera.
 * @version 0.0.1
 * @date 2021-06-29
 * 
 * @warning Under heavy development. Use at your own risk.
 * 
 */

/**
 * @mainpage Hello!
 * 
 * This is the main page!
 * 
 */

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
#define RFM95_DFLT_TX_POWER_dBm     2//5
#define RFM95_DFLT_SIGNAL_BANDWIDTH signalBandwidth_500kHz // signalBandwidth_250kHz
#define RH_RF95_MAX_MESSAGE_LEN 128
#define MIN_txPower 2
#define MAX_txPower 20

#define LINK_CHANGE_TIMEOUT_MILLIS 3000

#define USE_RH_RELIABLE_DATAGRAM true

//-----------------------------------------
// Message Type and Structure Declarations
//-----------------------------------------

/**
 * @brief Generic message structure.
 * 
 * @todo Replace buffer with c-string.
 * 
 */
struct message_t
{
  uint8_t srcAddr;
  uint8_t destAddr;
  uint8_t msgId;
  uint8_t flags;
  uint8_t bufLen;
  uint8_t buf [RH_RF95_MAX_MESSAGE_LEN];
};

/**
 * @brief Event tracing deprecated. Enum of traceable events left in in case tracing is re-enabled.
 * 
 */
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

/**
 * @brief Event tracing deprecated. Enum of statuses of traceable events left in in case tracing is re-enabled.
 * 
 */
enum eventStatus_t
{
  eventStatus_failed,
  eventStatus_success,
  NUM_eventStatuses
};

/**
 * @brief Enum of message types.
 * 
 * @todo Implement wake and sleep requests.
 * 
 */
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

/**
 * @brief Enum of available spreading factors. Serves as indices to spreadingFactorTable.
 * 
 * @todo Convert to class, add function or add operator to return value from spreadingFactorTable.
 * 
 */
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

/**
 * @brief Postfix increment operator for spreadingFactor_t. Circularly increments value, wrapping around from the max value.
 * 
 * @param s Spreading factor to increment.
 * @return spreadingFactor_t& Incremented spreading factor.
 */
spreadingFactor_t& operator++(spreadingFactor_t& s, int);

/**
 * @brief Enum of available signal bandwidths. Serves as indices to signalBandwidthTable.
 * 
 * @todo Convert to class, add function or add operator to return value from signalBandwidthTable.
 * 
 */
enum signalBandwidth_t
{
  signalBandwidth_125kHz,
  signalBandwidth_250kHz,
  signalBandwidth_500kHz,
  NUM_signalBandwidths
};

/**
 * @brief Postfix increment operator for signalBandwidth_t. Circularly increments value, wrapping around from the value of signalBandwidth_500kHz, thus it cannot access signalBandwidth_625kHz.
 * 
 * @param b Signal bandwidth to increment.
 * @return signalBandwidth_t& Incremented signal bandwidth.
 * 
 * @todo Test signalBandwidth_625kHz before adding it to increment operator. 
 * 
 */
signalBandwidth_t& operator++(signalBandwidth_t& b, int);

/**
 * @brief Enum of available frequency channels. Serves as indices to frequencyChannelTable.
 * 
 * @todo Convert to class, add function or add operator to return value from frequencyChannelTable.
 * 
 */
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

/**
 * @brief Postfix increment operator for frequencyChannel_t. Circularly increments value, wrapping around from the max value.
 * 
 * @param f Frequency channel to increment.
 * @return frequencyChannel_t& Incremented frequency channel.
* 
 */
frequencyChannel_t& operator++(frequencyChannel_t& f, int);

/**
 * @brief Struct of pointers to callback functions. This struct is defined in the file in which the loraPoint2Point object is constructed.
 * 
 */
struct userCallbacks_t
{
  void (*txInd) (uint8_t const * txBuf, 
                 uint8_t const bufLen, 
                 uint8_t const destAddr, 
                 bool ack);
  void (*rxInd) (message_t const & rxMsg);
  void (*linkChangeInd) (spreadingFactor_t const newSpreadingFactor,
                         signalBandwidth_t const newSignalBandwidth,
                         frequencyChannel_t const newFrequencyChannel,
                         int8_t const newTxPower);
};

//-----------------------
// Class Declarations
//-----------------------

/**
 * @brief Class handling LoRa point to point communication using the RFM95 radio module.
 * @note This class currently only supports the US915 frequency band.
 * @warning This class currently does not support automatic frequency hopping! Solutions using this are not guaranteed to comply with all regulatory requirements. For North-American users, refer to section 15.247 of the FCC's title 47, volume 1 regulations. This may be found at: https://www.govinfo.gov/content/pkg/CFR-2013-title47-vol1/pdf/CFR-2013-title47-vol1-sec15-247.pdf
 * 
 * This class uses the RadioHead packet library by AirSpayce for point-to-point communication.
 * RadioHead's documentation and downloads may be found at: http://www.airspayce.com/mikem/arduino/RadioHead/
 * This class is designed for the Arduino platform. Other compilers are not tested and will probably not work.
 * 
 *
 * This is the structure which the over the air transmissions follow:
 * All communications are acknowleged. Requests for info are followed by responses.
 * Indications of info are followed by confirmations.
 *
 * Client       Server   Client            Server
 * (Base)      (Sensor)  (Base)           (Sensor)
 *  |                |    |                     |
 *  |--Request(Req)->|    |<--Indication(Ind)---|
 *  |<-----Ack-------|    |---------Ack-------->|
 *  |<-Response(Rsp)-|    |--Confirmation(Cnf)->|
 *  |------Ack------>|    |<--------Ack---------|
 *  V                V    V                     V
 *
 * Client       Server   Client             Server
 * (Base)      (Sensor)  (Base)            (Sensor)
 *  |                |    |                      |
 *  |--Command(Cmd)->|    |<-Notification(Notif)-|
 *  |<-----Ack-------|    |----------Ack-------->|
 *  V                V    V                      V
 * 
 * @todo Support regions other than US915.
 * @todo Clean up class and put all members in alpabetical order.
 * @todo Add automatic frequency hopping.
 * @todo Add an option to use LoRaWAN or RadioHead's mesh networking system.
 * @todo Break out serial-port processing into a separate class.
 * @todo Add sleep and over-the-air wake functionality.
 * @todo Convert to Python module for CircuitPython users.
 * @todo Move enums and their operators into this class so that Doxygen documents them properly.
 */
class loraPoint2Point
{
  public:
    /**
     * @brief Constructs a new loraPoint2Point object.
     * 
     * @param _thisAddress  Unique address of the RFM95. This can be any 0-256 integer number.
     * @param rfm95CS       Arduino pinout location of RFM95 chip select pin.
     * @param rfm95Int      Arduino pinout location of RFM95 interrupt pin.
     * @param rfm95Rst      Arduino pinout location of RFM95 reset pin.
     * @param userCallbacks Struct of pointers to user callback functions.
     */
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
    
    //------------------
    // Public functions
    //------------------

    /**
     * @brief Get the fraction of data messages that have failed to receive an acknowlegement from their intended destination.
     * 
     * @return float const& Fraction from 0 to 1.
     */
    float const & getPacketErrorFraction ();

    /**
     * @brief Get the signal to noise ratio of the last received radio transmission.
     * 
     * @return int Signal to noise ratio of the last received radio transmission, in dB.
     */
    int getLastAckSNR ();

    /**
     * @brief Get the radio's current spreading factor setting.
     * 
     * @return spreadingFactor_t The current spreading factor.
     */
    spreadingFactor_t getSpreadingfactor ();

    /**
     * @brief Get the radio's current signal bandwidth setting.
     * 
     * @return signalBandwidth_t The current signal bandwidth.
     */
    signalBandwidth_t getSignalBandwidth ();

    /**
     * @brief Get the radio's current frequency channel setting.
     * 
     * @return frequencyChannel_t The current frequency channel.
     */    
    frequencyChannel_t getFrequencyChannel ();
    
    /**
     * @brief Get the radio's current transmission power setting.
     * 
     * @return int8_t The current transmission power, in dBm.
     * 
     * @note dBm is a decibel quantity calculated with respect to 1mW. i.e.; p_dBm = 10*log10(p_mW), p_mW = 10^(p_dBm/10)
     */
    int8_t getTxPower ();

    /**
     * @brief Initialize the radio and manager to default settings for US915. *Make sure to call this once in the application's setup function!*
     * 
     * @return true  Successful.
     * @return false Failed.
     * 
     * @todo Support other regions than US915.
     */
    bool setupRadio ();
    
    /**
     * @brief Set the radio's frequency channel.
     * 
     * Invalid inputs set the radio to RFM95_DFLT_FREQ_CHANNEL. Triggers a link change indication to the user.
     * 
     * @param frequencyChannel The frequency channel to which you wish to set this radio.
     * 
     * @todo Return instead of setting the radio to RFM95_DFLT_FREQ_CHANNEL. 
     */
    void setFrequencyChannel (frequencyChannel_t frequencyChannel);

    /**
     * @brief Set the radio's spreading factor.
     * 
     * Invalid inputs set the radio to RFM95_DFLT_SPREADING_FACTOR. Triggers a link change indication to the user.
     * 
     * @param spreadingFactor The spreading factor to which you wish to set this radio.
     */
    void setSpreadingFactor (spreadingFactor_t spreadingFactor);

    /**
     * @brief Set the radio's signal bandwidth.
     * 
     * Invalid inputs set the radio to RFM95_DFLT_SIGNAL_BANDWIDTH. Triggers a link change indication to the user.
     * 
     * @param bandwidth The signal bandwidth to which you wish to set this radio.
     */
    void setBandwidth (signalBandwidth_t bandwidth); 

    /**
     * @brief Set the radio's transmission power.
     * 
     * Invalid inputs set the radio to RFM95_DFLT_TX_POWER_dBm. Triggers a link change indication to the user.
     * 
     * @param txPower The transmission power to which you wish to set this radio, in dBm.
     */
    void setTxPower (int8_t txPower);

    /**
     * @brief Copy an arbitrary array of bytes into the TX message struct's buffer.
     * 
     * @param txMsgContents Pointer to an array of bytes.
     * @param numChars      Number of bytes to copy from the array.
     * @return uint8_t      Number of bytes copied into the buffer.
     * 
     * @warning **For debugging only.** Not guaranteed to be memory safe.
     * 
     * @todo Smarter way of doing this to avoid clobbering the contents of the txMessage.
     * @todo Replace with data request in application.
     */
    uint8_t setTxMessage (uint8_t const * txMsgContents,
                          uint8_t const numChars);
    
    /**
     * @brief Copies input from a serial port to the TX message struct's buffer.
     * 
     * Can also be used to trigger debug functionality following the subsequent 3 or 4 character command structure:
     *
     * `![Parameter][Value]`
     * 
     * And then entering a newline character (`\n`).
     * 
     * ! Signals the start of a debug command.
     * Parameter |      Setting       | Values  |
     * --------: | :----------------- | :------ |
     *         S | Spreading factor   | 0 to 10 |
     *         B | Signal bandwidth   | 0 to 3  |
     *         C | Frequency channel  | 0 to 15 |
     *         P | Transmission power | 1 to 20 |
     * 
     * @param dataPort The serial port (hardware UART object or USB serial object) which should be scanned.
     * @return uint8_t The number of characters added to the TX buffer.
     * 
     * @todo Test changing frequency channel above 9.
     * @todo Allow user to enter values from the settings' respective tables instead of entering the indices to those tables.
     * @todo Create a c-string, not an array of bytes.
     * @todo Write to a separate intermediate buffer instead of directly to the TX buffer to avoid wiping out the contents of the TX buffer.
     */
    uint8_t buildStringFromSerial (Serial_* dataPort);
    /**
     * @brief Copies input from a hardware UART serial port to the TX message struct's buffer. 
     * 
     * @overload
     */
    uint8_t buildStringFromSerial (Uart* dataPort);
    
    /**
     * @brief Transmits the current TX message's buffer contents to the specified destination.
     * 
     * @param destAddress The address of the destination, specified on that unit in the constructor of loraPoint2Point.
     */
    void serviceTx (uint8_t destAddress);

    /**
     * @brief Checks if there is a pending message. 
     * If so, acknowleges the message and calls the appropriate handler function.
     * Also handles timer ticking.
     * **Call this often to avoid missing messages.**
     * 
     */
    void serviceRx ();
    
    /**
     * @brief Requests a data message to be sent to a given destination.
     * 
     * @todo Complete prototype and definition.
     */
    void dataReq (uint8_t const  destAddress,
                  uint8_t* const buf,
                  uint8_t const  numChars);
    
    /**
     * @brief Requests that both the client and the server change their radio settings to those provided.
     *
     * It works like this:
     * 1. After recieving the request, the server acknowleges the transmission, changes its radio settings, and waits for a short delay.
     * 2. Upon recieving the acknowlegement, the client changes its radio settings.
     * 3. After both change their radio settings, the server sends a response to test the new link settings.
     * 4. If the client does not recieve the response in 3 seconds, it reverts its settings.
     * 5. Upon recieving the response, the client stops and resets the timer that will cause it to revert its settings on timeout.
     * 6. If the server does not recieve the acknowlegement, the server reverts its settings.
     * 
     * @todo If the server does not recieve the response-acknowlegement, it should send an indication for the client to reset its radio settings before resetting its own settings. This will prevent a situation in which the client changes its settings but the server resets its own settings.
     * 
     * @param destAddress      The address of the other unit (the server) that should have its link changed.
     * @param spreadingFactor  The new spreading factor (0-5) that both units should adopt.
     * @param signalBandwidth  The new signal bandwidth (0-3) that both units should adopt.
     * @param frequencyChannel The new frequency channel (0-15) that both units should adopt.
     * @param txPower          The new transmission power (1-20dBm) that both units should adopt.
     */
    void linkChangeReq  (uint8_t const            destAddress,
                         spreadingFactor_t const  spreadingFactor,
                         signalBandwidth_t const  signalBandwidth,
                         frequencyChannel_t const frequencyChannel,
                         int8_t const             txPower);
    
    /**
     * @brief Advance and check all timers in loraPoint2Point.
     * 
     * This is automatically called in serviceRx, but if serviceRx is not called for whatever reason, call this regularly in the main loop.
     */
    void serviceTimers ();
    
    //sendLinkChangeRequest (uint8_t destAddr;
    //                       spreadingFactor_t spreadingFactor,
    //                       signalBandwidth_t signalBandwidth,
    //                       int8_t txPower);
    //checkForMessages ();
    //sendSleepRequest (uint8_t destAddr);
    //sendWakeRequest (uint8_t destAddr);
    //goToSleep ();

  private:

    //-------------------
    // Private variables
    //-------------------

    int ackSnr = 0;
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
    const uint32_t signalBandwidthTable [NUM_signalBandwidths] = {125000, 250000, 500000};
    const uint16_t frequencyChannelTable [NUM_frequencyChannels] = {9030, 9046, 9062, 9078, 9094, 9110, 9126, 9142, 9233, 9239, 9245, 9251, 9257, 9263, 9269, 9275};
    uint32_t currentMillis = 0;
    float packetErrorFraction = 0;
    uint32_t packetCount = 0;

    //-----------------
    // Private classes
    //-----------------

    RH_RF95 rf95;
    RHReliableDatagram rhReliableDatagram;
    simpleTimer linkChangeTimeoutTimer = simpleTimer(LINK_CHANGE_TIMEOUT_MILLIS,
                                                     currentMillis,
                                                     false);
    userCallbacks_t user;
    //list<simpleTimer*> simpleTimerList;
    
    //-------------------
    // Private functions
    //-------------------
    
    /**
     * @brief Processes input from the serial port.
     * Either calling debug commands as described in buildStringFromSerial or otherwise building a string in txBuffer.
     * Called once for every character in the serial buffer. 
     * 
     * @param inputChar The current character on the serial buffer.
     * @return uint8_t  The number of characters in the txBuffer.
     */
    uint8_t buildStringFromSerialInner (char inputChar);

    /**
     * @brief Forces a reset on the RFM95 by writing the reset line low for 10ms then raising it.
     * 
     */
    void forceRadioReset ();

    /**
     * @brief Resets radio settings to previous values if 3s have elapsed without recieving a link change response.
     * 
     */
    void linkChangeReqTimeout ();

    /**
     * @brief Resets current packet error fraction to 0%.
     * 
     */
    void resetPacketErrorFraction ();

    /**
     * @brief Handler to be called in the event that a unit recieves a data request.
     * @note Currently not implemented.
     */
    void serviceDataReq ();
    
    /**
     * @brief Handler to be called in the event that a unit recieves a response back to a data request.
     * @note Currently not implemented.
     */
    void serviceDataRsp ();

    /**
     * @brief Handler to be called in the event that a unit recieves a link change request.
     * See linkChangeReq for details.
     * 
     * @param srcAddress       The address of the unit requesting the link change.
     * @param spreadingFactor  The spreading factor being requested.
     * @param signalBandwidth  The signal bandwidth being requested.
     * @param frequencyChannel The frequency channel being requested.
     * @param txPower          The TX power being requested.
     * 
     * @related linkChangeReq
     */
    void serviceLinkChangeReq (uint8_t const            srcAddress,
                               spreadingFactor_t const  spreadingFactor,
                               signalBandwidth_t const  signalBandwidth,
                               frequencyChannel_t const frequencyChannel,
                               int8_t const             txPower);
    /**
     * @brief Handler to be called in the event that a unit recieves a link change response.
     * See linkChangeReq for details.
     * 
     * @related linkChangeReq
     */
    void serviceLinkChangeRsp ();

    /**
     * @brief Adds to packet error fraction if packet is unsuccessful.
     * 
     * @param packetSuccess True if successful.
     */
    void updatePacketErrorFraction (bool packetSuccess);    
};

#endif // LORA_POINT_2_POINT_PROTOCOL_H