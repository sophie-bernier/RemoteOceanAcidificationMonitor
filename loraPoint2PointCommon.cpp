/**
 * @file loraPoint2PointCommon.cpp
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Functions common to standard and lightweight loraPoint2PointProtocol.
 * @version 0.1
 * @date 2021-07-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <loraPoint2PointCommon.h>

void loraPoint2PointCommon::printBuffer (uint8_t const * buf,
                                   uint8_t const bufLen,
                                   bool const ascii)
{
  if (ascii)
  {
    for (uint8_t i = 0; i < bufLen; i++)
    {
      Serial.print(char(buf[i]));
    }
  }
  else
  {
    Serial.print("0x");
    for (uint8_t i = 0; i < bufLen; i++)
    {
      if (buf[i] < 0x10)
      {
        Serial.print('0');
      }
      Serial.print(buf[i], HEX);
    }
  }
}
