/**
 * @file sensors.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
//#include <proO.h>

enum sensors_t
{
  sensor_none,
  sensor_proCV,
  sensor_seapHOx,
  NUM_sensors
};

/*
class Sensors
{
  public:
    //seapHOx seapHOx;
    ProCV ProCV;
}
*/

#endif SENSORS_H