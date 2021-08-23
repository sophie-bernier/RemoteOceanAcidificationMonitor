@file README
@author Sophie Bernier (raptoronabicycle@gmail.com)
@brief 
@version 0.1
@date 2021-08-23

@copyright Copyright (c) 2021
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


@mainpage Remote Ocean Acidification Monitor Project

This open-source hardware and software library is designed to show how the Arduino ecosystem can be used to interface oceanographic sensors with inexpensive data logging and communications systems. The examples in this library are meant to interface the [Seabird SeapHOx](https://www.seabird.com/seaphoxv2) dissolved O<sub>2</sub> sensor and the [Pro-Oceanus CO<sub>2</sub>-Pro CV](https://pro-oceanus.com/products/pro-series/co2-pro-cv) dissolved CO<sub>2</sub> sensor with the [Adafruit Feather M0 RFM95](https://www.adafruit.com/product/3178) LoRa digital packet radio.

This project owes its existence to the support of [Dr. Brent Else](https://www.ucalgary.ca/research/scholars/else-brent), [Dr. Kristina Barclay](https://www.researchgate.net/profile/Kristina-Barclay), Dr. Richard Simms, and the other researchers working at the University of Calgary and the [Marine Environmental Observation, Prediction and Response Network's Ocean Acidification Community of Practice](https://meopar.ca/) to study the exchange of greenhouse gases between the air and lakes, rivers, and oceans. Funding for this project was provided by the Natural Sciences and Engineering Research Council of Canada.

This library includes the following examples:
* LoRaInterfaceForwardingTest (LoRaInterfaceForwardingTest_Base.ino and LoRaInterfaceForwardingTest_Endpoint.ino): Attempt to use LoraPoint2PointProtocol, in an application based on LoRaRangeTest, to talk with sensors over radio. CURRENTLY UNRELIABLE due to race conditions between base and endpoint, use simpleSensorCommsEchoRadio to talk with the sensors over radio instead.
* simpleSensorCommsEchoRadio (simpleSensorCommsEchoRadio_Base.ino) and simpleSensorCommsEchoRadio_Endpoint.ino): Simplest possible implementation of radio communications interface between shore-based base and sensor-connected endpoints.
* KiCAD schematic and PCB layout (Shield_PCB_Feather) for sensor interface (TTL UART serial to/from RS-232 serial) and to power the Feather off of the same 12V external supply as the sensors.
* Serial_1_2 (Serial_1_2_RX.ino and Serial_1_2_TX.ino): How to add an additional hardware UART port on the Adafruit Feather (and other Atmel SAMD21 based Arduino-compatible boards) using the unused SERCOM (serial communication) peripherals.
* LoRaRangeTest (LoRaRangeTest_Base.ino and  LoRaRangeTest_Endpoint.ino): Uses the Adafruit Adalogger and 128x32 OLED shield to turn the sensor endpoint into a mobile range testing and evaluation unit that can change the endpoint's radio settings remotely.
* Misc. LoRa and serial examples for pycom LoPy4 boards.

All of these examples are short and simple, so additions and changes to the code should be straightforward. 

The [Tests](\Tests) folder contains additional examples of work-in-progress features such locally emulating the sensor interface.

Additional documentation, including testing and problem solving reports, can be requested from Brent Else's lab OneDrive.



