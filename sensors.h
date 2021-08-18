/**
 * @file sensors.h
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Meant as a container class for emulated sensor interfaces. Currently just holds a list of the sensors.
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