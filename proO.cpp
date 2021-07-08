/**
 * @file proO.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <proO.h>

#define FLOAT_2_INT_AND_FRAC(floatIn, intOut, fracOut, precision) \
intOut = (unsigned int)(floatIn);\
fracOut = (floatIn)*(precision) - ((unsigned int)(floatIn))*(precision)

#define INT_AND_FRAC_2_FLOAT(floatOut, intIn, fracIn, precision) \
floatOut = (intIn) + ((float)(fracIn))/(precision)

//-----------
// ProCVData
//-----------

//------------------
// Public Functions
//------------------

ProCVDataBitfield const & ProCVData::getBitfieldData ()
{
  return bitfieldData;
}

ProCVDataReadable const & ProCVData::getReadableData ()
{
  return readableData;
}

void ProCVData::setDataFromString (String const & DataString)
{
  readableData.year   = getValueFromDataString(DataString, true).toInt();
  readableData.month  = getValueFromDataString(DataString, false).toInt();
  readableData.day    = getValueFromDataString(DataString, false).toInt();
  readableData.hour   = getValueFromDataString(DataString, false).toInt();
  readableData.minute = getValueFromDataString(DataString, false).toInt();
  readableData.second = getValueFromDataString(DataString, false).toInt();
  readableData.zeroAD = getValueFromDataString(DataString, false).toInt();
  readableData.currentAD = getValueFromDataString(DataString, false).toInt();
  readableData.CO2_ppm = getValueFromDataString(DataString, false).toFloat();
  readableData.avgIrgaTemperature_C = getValueFromDataString(DataString, false).toFloat();
  readableData.humidity_mbar = getValueFromDataString(DataString, false).toFloat();
  readableData.humiditySensorTemperature_C = getValueFromDataString(DataString, false).toFloat();
  readableData.cellGasPressure_mbar = getValueFromDataString(DataString, false).toInt();
  readableData.supply_volts = getValueFromDataString(DataString, false).toFloat();
}

void ProCVData::printAllData (Serial_ & debugSerial)
{
  if (!debugSerial)
  {
    return;
  }
  debugSerial.print("readableData is ");
  debugSerial.print(sizeof(readableData));
  debugSerial.println(" bytes");
  debugSerial.print("year: ");
  debugSerial.println(readableData.year);
  debugSerial.print("month: ");
  debugSerial.println(readableData.month);
  debugSerial.print("day: ");
  debugSerial.println(readableData.day);
  debugSerial.print("hour: ");
  debugSerial.println(readableData.hour);
  debugSerial.print("minute: ");
  debugSerial.println(readableData.minute);
  debugSerial.print("second: ");
  debugSerial.println(readableData.second);
  debugSerial.print("zeroAD: ");
  debugSerial.println(readableData.zeroAD);
  debugSerial.print("currentAD: ");
  debugSerial.println(readableData.currentAD);
  debugSerial.print("CO2_ppm: ");
  debugSerial.println(readableData.CO2_ppm);
  debugSerial.print("avgIrgaTemperature_C: ");
  debugSerial.println(readableData.avgIrgaTemperature_C);
  debugSerial.print("humidity_mbar: ");
  debugSerial.println(readableData.humidity_mbar);
  debugSerial.print("humiditySensorTemperature_C: ");
  debugSerial.println(readableData.humiditySensorTemperature_C);
  debugSerial.print("cellGasPressure_mbar: ");
  debugSerial.println(readableData.cellGasPressure_mbar);
  debugSerial.print("supply_volts: ");
  debugSerial.println(readableData.supply_volts);
}

void ProCVData::printAllBitfieldData (Serial_ & debugSerial)
{
  if (!debugSerial)
  {
    return;
  }
  debugSerial.print("bitfieldData is ");
  debugSerial.print(sizeof(bitfieldData));
  debugSerial.println(" bytes");
  debugSerial.print("year: ");
  debugSerial.println(bitfieldData.year);
  debugSerial.print("month: ");
  debugSerial.println(bitfieldData.month);
  debugSerial.print("day: ");
  debugSerial.println(bitfieldData.day);
  debugSerial.print("hour: ");
  debugSerial.println(bitfieldData.hour);
  debugSerial.print("minute: ");
  debugSerial.println(bitfieldData.minute);
  debugSerial.print("second: ");
  debugSerial.println(bitfieldData.second);
  debugSerial.print("zeroAD: ");
  debugSerial.println(bitfieldData.zeroAD);
  debugSerial.print("currentAD: ");
  debugSerial.println(bitfieldData.currentAD);
  debugSerial.print("CO2_ppm_int: ");
  debugSerial.println(bitfieldData.CO2_ppm_int);
  debugSerial.print("CO2_ppm_frac: ");
  debugSerial.println(bitfieldData.CO2_ppm_frac);
  debugSerial.print("avgIrgaTemperature_C_int: ");
  debugSerial.println(bitfieldData.avgIrgaTemperature_C_int);
  debugSerial.print("avgIrgaTemperature_C_frac: ");
  debugSerial.println(bitfieldData.avgIrgaTemperature_C_frac);
  debugSerial.print("humidity_mbar_int: ");
  debugSerial.println(bitfieldData.humidity_mbar_int);
  debugSerial.print("humidity_mbar_frac: ");
  debugSerial.println(bitfieldData.humidity_mbar_frac);
  debugSerial.print("humiditySensorTemperature_C_int: ");
  debugSerial.println(bitfieldData.humiditySensorTemperature_C_int);
  debugSerial.print("humiditySensorTemperature_C_frac: ");
  debugSerial.println(bitfieldData.humiditySensorTemperature_C_frac);
  debugSerial.print("cellGasPressure_mbar: ");
  debugSerial.println(bitfieldData.cellGasPressure_mbar);
  debugSerial.print("supply_volts_int: ");
  debugSerial.println(bitfieldData.supply_volts_int);
  debugSerial.print("supply_volts_frac: ");
  debugSerial.println(bitfieldData.supply_volts_frac);
}

//-------------------
// Private Functions
//-------------------

void ProCVData::convertBitfieldToReadable ()
{
  readableData.year = bitfieldData.year;
  readableData.month = bitfieldData.month;
  readableData.day = bitfieldData.day;
  readableData.hour = bitfieldData.hour;
  readableData.minute = bitfieldData.minute;
  readableData.second = bitfieldData.second;
  readableData.zeroAD = bitfieldData.zeroAD;
  readableData.currentAD = bitfieldData.currentAD;
  INT_AND_FRAC_2_FLOAT(readableData.CO2_ppm, bitfieldData.CO2_ppm_int, bitfieldData.CO2_ppm_frac, PRECISION_CO2_PPM);
  INT_AND_FRAC_2_FLOAT(readableData.avgIrgaTemperature_C, bitfieldData.avgIrgaTemperature_C_int, bitfieldData.avgIrgaTemperature_C_frac, PRECISION_IRGA_TEMPERATURE);
  INT_AND_FRAC_2_FLOAT(readableData.humidity_mbar, bitfieldData.humidity_mbar_int, bitfieldData.humidity_mbar_frac, PRECISION_HUMIDITY);
  INT_AND_FRAC_2_FLOAT(readableData.humiditySensorTemperature_C, bitfieldData.humiditySensorTemperature_C_int, bitfieldData.humiditySensorTemperature_C_frac, PRECISION_HUMIDITY_SENSOR_TEMPERATURE);
  readableData.cellGasPressure_mbar = bitfieldData.cellGasPressure_mbar;
  INT_AND_FRAC_2_FLOAT(readableData.supply_volts, bitfieldData.supply_volts_int, bitfieldData.supply_volts_frac, PRECISION_SUPPLY_VOLTS);
}

void ProCVData::convertReadableToBitfield ()
{
  
  bitfieldData.year = readableData.year;
  bitfieldData.month = readableData.month;
  bitfieldData.day = readableData.day;
  bitfieldData.hour = readableData.hour;
  bitfieldData.minute = readableData.minute;
  bitfieldData.second = readableData.second;
  bitfieldData.zeroAD = readableData.zeroAD;
  bitfieldData.currentAD = readableData.currentAD;
  FLOAT_2_INT_AND_FRAC(readableData.CO2_ppm, bitfieldData.CO2_ppm_int, bitfieldData.CO2_ppm_frac, PRECISION_CO2_PPM);
  FLOAT_2_INT_AND_FRAC(readableData.avgIrgaTemperature_C, bitfieldData.avgIrgaTemperature_C_int, bitfieldData.avgIrgaTemperature_C_frac, PRECISION_IRGA_TEMPERATURE);
  FLOAT_2_INT_AND_FRAC(readableData.humidity_mbar, bitfieldData.humidity_mbar_int, bitfieldData.humidity_mbar_frac, PRECISION_HUMIDITY);
  FLOAT_2_INT_AND_FRAC(readableData.humiditySensorTemperature_C, bitfieldData.humiditySensorTemperature_C_int, bitfieldData.humiditySensorTemperature_C_frac, PRECISION_HUMIDITY_SENSOR_TEMPERATURE);
  bitfieldData.cellGasPressure_mbar = readableData.cellGasPressure_mbar;
  FLOAT_2_INT_AND_FRAC(readableData.supply_volts, bitfieldData.supply_volts_int, bitfieldData.supply_volts_frac, PRECISION_SUPPLY_VOLTS);
}


String ProCVData::getValueFromDataString (String const & DataString, bool resetIdx)
{
  String Buffer;
  static int16_t leadingIdx; // Starts with "W M," - 4 bytes
  static int16_t trailingIdx;
  if (resetIdx)
  {
    leadingIdx = 4;
    trailingIdx = 4;
  }
  leadingIdx = DataString.indexOf(',', trailingIdx);
  if (leadingIdx < 0)
  {
    Buffer = DataString.substring(trailingIdx);
    leadingIdx = 4;
    trailingIdx = 4;
  }
  else
  {
    Buffer = DataString.substring(trailingIdx, leadingIdx);
    trailingIdx = leadingIdx + 1;
  }
  //Serial.println(Buffer);
  return Buffer;
}

//-------
// ProCV
//-------

//-------------------
// Private Functions
//-------------------

void ProCV::sendEsc ()
{
  Serial.println("DBG: Sending 'ESC'");
  Serial1.write(27);
  lastTXMillis = millis();
}

//------------------
// Public Functions
//------------------k


void ProCV::serviceSerial ()
{
  if (dataPort.available()) 
  {
    rxBuffer = dataPort.readStringUntil('\n');
    currentRXMillis = millis();
    newRx = true;
    Serial.println(rxBuffer);
    Serial.print("DBG: ");
    Serial.print(currentRXMillis - lastRXMillis);
    Serial.print("ms since RX, ");
    Serial.print(currentRXMillis - lastTXMillis);
    Serial.println("ms since TX");
    lastRXMillis = currentRXMillis;
  }

  switch (state)
  {
    case state_asleep:
      switch (command)
      {
        case command_startViewingLoggedData:
          sendEsc();
          state = state_idle;
        case command_wakeup:
          sendEsc();
          state = state_idle;
          command = command_none;
      }
      break;
    case state_idle:
      if (currentMillis - lastTXMillis > 1000) // time for status to show
      {
        sendEsc();
        state = state_menu;
      }
      break;
    case state_menu:
      if (currentMillis - lastTXMillis > 1000) // time for menu to show
      {
        switch (command)
        {
          case command_startViewingLoggedData:
            dataPort.print("4\r");
            lastTXMillis = millis();
            state = state_viewLoggedData;
            command = command_none;
        }
      }
      /*
      if (currentMillis - lastTXMillis > 10000) // debug - start viewing data after a while
      {
        command = command_startViewingLoggedData;
      }
      */
      if (currentMillis - lastTXMillis > 60000)
      {
        state = state_asleep;
        //command = command_wakeup; debug - wake as soon as it goes to sleep
      }

      break;
    case state_viewLoggedData:
      if (newRx)
      {
        dataPort.setDataFromString(rxBuffer);
        // Data.printAllData(Serial);
      }
      /*
      if (currentMillis - lastTXMillis > 20000) // debug - stop viewing data after a while
      {
        command = command_stopViewingLoggedData;
      }
      */
      if (currentMillis - lastRXMillis > 1000) // accept new commands as sensor returns to menu  when reading is done
      {
        state = state_menu;
      }
      switch (command)
      {
        case command_stopViewingLoggedData:
          state = state_idle; // send esc and return to main menu
      }
      break;
    default:
      break;
  }
}

void ProCV::startViewingLoggedData ()
{
  command = command_startViewingLoggedData;
}

void ProCV::stopViewingLoggedData ()
{
  command = command_stopViewingLoggedData;
}

void wakeSensor ()
{
  command = command_wakeup;
}