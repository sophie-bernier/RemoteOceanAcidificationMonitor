// loraPoint2PointProtocol.cpp

#include <Arduino.h>
#include "loraPoint2PointProtocol.h"

loraPoint2Point

sendLinkAdapt ()


uint8_t loraPoint2Point::serviceMsg (uint8_t* buf, 
                                     uint8_t* len,
                                     uint8_t* from,
                                     uint8_t* to,
                                     uint8_t* id,
                                     uint8_t* flags)
{
  switch (buf[0])
  {
    case msgType_data:
      serviceMsgData(buf[1], len - 1, from, to, id, flags);
      break;
    case msgType_linkAdapt:
      serviceMsgLinkAdapt(buf[1], len - 1);
      break;
  }
  return 0;
}



Hello World!