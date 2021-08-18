/**
 * @file loraPoint2PointCommon.h
 * @author your name (you@domain.com)
 * @brief Functions common to standard and lightweight loraPoint2PointProtoocl
 * @version 0.1
 * @date 2021-07-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef LORA_POINT_2_POINT_COMMON_H
#define LORA_POINT_2_POINT_PROTOCOL_H

#include <Arduino.h>


namespace loraPoint2PointCommon
{
/**
 * @brief Quick way of printing an array of bytes.
 * 
 * @param buf    Pointer to the array of bytes to print.
 * @param bufLen Number of bytes to print.
 * @param ascii  True: Prints buffer as characters. False: Prints buffer as hexadecimal number.
 */
void printBuffer (uint8_t const * buf,
                  uint8_t const bufLen,
                  bool const ascii);
}
#endif // LORA_POINT_2_POINT_PROTOCOL_H