/**
 * @file loraPoint2PointProtocolLightweight.h
 * @author your name (you@domain.com)
 * @brief Less overhead for high datarate applications.
 * @version 0.1
 * @date 2021-07-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#ifndef LORA_POINT_2_POINT_LIGHTWEIGHT
#define LORA_POINT_2_POINT_LIGHTWEIGHT

/**
 * @brief Enum of message types.
 * 
 * @todo Implement wake and sleep requests.
 * 
 */
enum msgType_t
{
  msgType_undefined,     // 0
  msgType_dataReq,       // 1
  msgType_dataRsp,       // 2
  msgType_linkChangeReq, // 3, unsupported in Lightweight
  msgType_linkChangeRsp, // 4, unsupported in Lightweight
  msgType_wakeRequest,   // 5, unsupported in Lightweight
  msgType_sleepRequest,  // 6, unsupported in Lightweight
  msgType_heartbeatReq,  // 7, unsupported in Lightweight
  msgType_heartbeatRsp,  // 8, unsupported in Lightweight
  msgType_procvDataReq,  // 9
  msgType_procvDataRsp,  // 10
  msgType_seaphoxDataReq,// 11
  msgType_seaphoxDataRsp,// 12
  NUM_msgTypes           // 13
};

String const msgTypeNames [] {"undefined message type", // 0
                                    "dataReq",       // 1
                                    "dataRsp",       // 2
                                    "linkChangeReq", // 3
                                    "linkChangeRsp", // 4
                                    "wakeRequest",   // 5
                                    "sleepRequest",  // 6
                                    "heartbeatReq",  // 7
                                    "heartbeatRsp",  // 8
                                    "procvDataReq",  // 9
                                    "procvDataRsp",  // 10
                                    "seaphoxDataReq", // 11
                                    "seaphoxDataRsp"}; // 12

#endif // LORA_POINT_2_POINT_LIGHTWEIGHT