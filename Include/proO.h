/**
 * ProO.h
 * By Sophie Bernier
 *  2021-05-21
 * Last Modified 2021-05-21
 * Library to handle communication with the
 * Pro-Oceanus CO2 Pro CV sensor.
 */

enum sampleMode_t
{
  sampleMode_continuous,
  sampleMode_timed,
  sampleMode_command
};

enum dataField_t
{
  dataField_year,
  dataField_month,
  dataField_day,
  dataField_hour,
  dataField_minute,
  dataField_second,
  dataField_zeroAD,
  dataField_currentAD,
  dataField_CO2_int,
  dataField_CO2_frac,
  dataField_AvgIrgaTemp_int,
  dataField_AvgIrgaTemp_frac,
  dataField_Humidity_int,
  dataField_Humidity_frac,
};

#define DATA_FIELD_YEAR_BITS

class ProCVData
{
  private:
   uint8_t* dataBuffer;
   uint8_t dataBufferIdx;
   uint8_t addToBuffer (uint8_t);
};

/* addToBuffer pseudocode
uint8_t addToBuffer (uint8_t *incomingChar)
{
  
  
  incomingDigit = incomingChar - 0x30;
  dataField += incomingDigit*10^i;
  return success
}
*/

class ProCV
{
  private:
    Serial DevSerial;
    ProCVData Data;
  protected:
  public:
    ProCV (Serial);
    uint8_t startSensor ();
    uint8_t startSampling ();
    uint8_t readData ();
};



uint8_t ProCV::startSampling ();
uint8_t ProCV::stopSampling ();

uint8_t ProCV::setSampleModeContinuous (uint16_t samplesToSkip,
                                        bool     clearZeroCount,
                                        uint8_t  zeroIntervalHours);
uint8_t ProCV::setSampleModeTimed (uint16_t sampleIntervalMins,
                                   uint8_t  readingsPerSample,
                                   uint8_t  firstSampleHour,
                                   uint8_t  firstSampleMin,
                                   uint8_t  firstSampleSec,
                                   bool     logAverage,
                                   bool     clearZeroCount,
                                   uint8_t  zeroIntervalHours);
uint8_t ProCV::setSampleModeCommand (uint8_t  readingsPerSample,
                                     bool     logAverage,
                                     bool     clearZeroCount,
                                     uint8_t  zeroIntervalHours);
uint8_t ProCV::viewLoggedData ();
uint8_t ProCV::setLoggedDataLineCallback ();
uint8_t ProCV::setLoggedDataSubstrCallback ();
uint8_t ProCV::eraseLoggedData ();
uint8_t ProCV::getStatus ();
uint8_t ProCV::setClockTime ();
uint8_t ProCV::setBaudRate ();
uint8_t ProCV::restoreFactoryDefault ();
uint8_t ProCV::scheduleZero ();
uint8_t ProCV::startSingleSample ();

