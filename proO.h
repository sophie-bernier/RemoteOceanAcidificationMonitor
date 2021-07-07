/**
 * @file proO.h
 * @author Sophie Bernier (raptoronabicycle@gmail.com)
 * @brief Library to handle communication with the Pro-Oceanus CO2 Pro CV sensor.
 * @version 0.1
 * @date 2021-07-02
 * Created 2021-05-21
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <SimpleTimer.h>

#define ESC ((char)(27))

enum sampleMode_t
{
  sampleMode_continuous,
  sampleMode_timed,
  sampleMode_command
};
/*
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
*/
/*
#define MAX_YEAR 
#define MAX_MONTH 
#define MAX_DAY 
#define MAX_HOUR 
#define MAX_MINUTE 
#define MAX_SECOND 
#define MAX_ZERO_AD 
#define MAX_CURRENT_AD 
#define MAX_CO2_PPM_INT 
#define MAX_CO2_PPM_FRAC 
#define MAX_AVG_IRGA_TEMPERATURE_C_INT 
#define MAX_AVG_IRGA_TEMPERATURE_C_FRAC 
#define MAX_HUMIDITY_MBAR_INT 
#define MAX_HUMIDITY_MBAR_FRAC 
#define MAX_CELL_GAS_PRESSURE_MBAR 
#define MAX_SUPPLY_VOLTS_INT 
#define MAX_SUPPLY_VOLTS_FRAC 
*/

#define PRECISION_CO2_PPM 100 // 2 decimal digits
#define PRECISION_IRGA_TEMPERATURE 10 // 1 decimal digit
#define PRECISION_HUMIDITY 100 // 2 decimal digits
#define PRECISION_HUMIDITY_SENSOR_TEMPERATURE 100 // 2 decimal digits
#define PRECISION_SUPPLY_VOLTS 10 // 1 decimal digit

#define BITS_YEAR  12
#define BITS_MONTH 4
#define BITS_DAY 5
#define BITS_HOUR 4
#define BITS_MINUTE 6
#define BITS_SECOND 6
#define BITS_ZERO_AD 16
#define BITS_CURRENT_AD 16
#define BITS_CO2_PPM_INT 10
#define BITS_CO2_PPM_FRAC 7
#define BITS_AVG_IRGA_TEMPERATURE_C_INT 6
#define BITS_AVG_IRGA_TEMPERATURE_C_FRAC 4
#define BITS_HUMIDITY_MBAR_INT 6
#define BITS_HUMIDITY_MBAR_FRAC 7
#define BITS_HUMIDITY_SENSOR_TEMPERATURE_C_INT 6
#define BITS_HUMIDITY_SENSOR_TEMPERATURE_C_FRAC 7
#define BITS_CELL_GAS_PRESSURE_MBAR 11
#define BITS_SUPPLY_VOLTS_INT 4
#define BITS_SUPPLY_VOLTS_FRAC 4

enum nextCommand_t
{
  nextCommand_none,
  nextCommand_startViewingLoggedData
};

enum expecting_t
{
  expecting_undefined,
  expecting_loggedData,
  NUM_expecting
};

struct ProCVDataBitfield
{
  unsigned int year                      : BITS_YEAR;
  unsigned int month                     : BITS_MONTH;
  unsigned int day                       : BITS_DAY;
  unsigned int hour                      : BITS_HOUR;
  unsigned int minute                    : BITS_MINUTE;
  unsigned int second                    : BITS_SECOND;
  unsigned int zeroAD                    : BITS_ZERO_AD;
  unsigned int currentAD                 : BITS_CURRENT_AD;
  unsigned int CO2_ppm_int               : BITS_CO2_PPM_INT;
  unsigned int CO2_ppm_frac              : BITS_CO2_PPM_FRAC;
  unsigned int avgIrgaTemperature_C_int  : BITS_AVG_IRGA_TEMPERATURE_C_INT;
  unsigned int avgIrgaTemperature_C_frac : BITS_AVG_IRGA_TEMPERATURE_C_FRAC;
  unsigned int humidity_mbar_int         : BITS_HUMIDITY_MBAR_INT;
  unsigned int humidity_mbar_frac        : BITS_HUMIDITY_MBAR_FRAC;
  unsigned int humiditySensorTemperature_C_int  : BITS_HUMIDITY_SENSOR_TEMPERATURE_C_INT;
  unsigned int humiditySensorTemperature_C_frac : BITS_HUMIDITY_SENSOR_TEMPERATURE_C_FRAC;
  unsigned int cellGasPressure_mbar      : BITS_CELL_GAS_PRESSURE_MBAR;
  unsigned int supply_volts_int          : BITS_SUPPLY_VOLTS_INT;
  unsigned int supply_volts_frac         : BITS_SUPPLY_VOLTS_FRAC;
};

struct ProCVDataReadable
{
  uint8_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  uint16_t zeroAD;
  uint16_t currentAD;
  float    CO2_ppm;
  float    avgIrgaTemperature_C;
  float    humidity_mbar;
  float    humiditySensorTemperature_C;
  uint16_t cellGasPressure_mbar;
  float    supply_volts;
};

class ProCVData
{
  public:
    ProCVDataBitfield const & getBitfieldData ();
    ProCVDataReadable const & getReadableData ();
    void setDataFromString (String const & DataString);
    void printAllData (Serial_ & debugSerial);
    void printAllBitfieldData (Serial_ & debugSerial);
    void convertBitfieldToReadable ();
    void convertReadableToBitfield ();
  private:
    ProCVDataReadable readableData = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    ProCVDataBitfield bitfieldData = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    String getValueFromDataString (String const & DataString, bool resetIdx);
   //uint8_t* dataBuffer;
   //uint8_t dataBufferIdx;
   //uint8_t addToBuffer (uint8_t);
};

/* addToBuffer pseudocode
uint8_t addToBuffer (uint8_t *incomingChar)
{
  incomingDigit = incomingChar - 0x30;
  dataField += incomingDigit*10^i;
  return success
}
*/

struct ProCVCallbacks_t
{
  // void (*dataNotif) (ProCVData const & dataPoint);
};

class ProCV
{
  private:
    Uart dataPort;
    ProCVData Data;
    uint32_t firmwareVersion;
    uint16_t currentYear;
    uint8_t currentMonth;
    uint8_t currentDay;
    uint8_t currentHour;
    uint8_t currentMinute;
    uint8_t currentSecond;
    uint16_t supplyMilliVolts;
    uint16_t baudRate;
    sampleMode_t sampleMode; 
    uint32_t currentMillis = 0;
    simpleTimer commandDelayTimer = simpleTimer(1000,
                                                currentMillis,
                                                false);
    uint16_t escCount = 0;
    nextCommand_t nextCommand = nextCommand_none;
    expecting_t expecting = expecting_undefined;
    String inputBuffer;

    void sendEsc ();
    void echoFromSerial ();
    void purgeSerial ();
  protected:
  public:
    /**
     * @brief Constructs a new ProCV object.
     * 
     * @param dataPort Hardware serial port to which the CO2 Pro CV is attached.
     * 
     */
    ProCV (Uart & _dataPort, ProCVCallbacks_t _callbacks):dataPort{_dataPort}{}
    
    /**
     * @brief Check for incoming data from the sensor on the serial port. Run this often in the main loop to ensure that serial messages are not missed and that the serial buffer does not overflow!
     * 
     */
    void serviceSerial ();

    /**
     * @brief Initializes sensor by sending two 'ESC' (0x1b) characters.
     * 
     * @return true  Sensor started successfully.
     * @return false Sensor startup failed.
     * Entering y/n values just requires entering a 'y' or a 'n' character sometimes, but it doesn't hurt to enter an '\r' (0x0d) character after.
     * Entering a numerical value requires entering an '\r' (0x0d) character after the number.
     * Expect from sensor (after every command):
     * @verbatim
      
      **************************************************************************************
        2) STOP sampling                      7) Sensor status
        3) Setup sampling
      
        4) View logged data                   t) Set clock time
        5) Erase logged data                  b) Set baud rate
      
        z) Schedule a zero for next sample    s) Single sample acquisition in command mode
      ***************************************************************************************
      
      >>
       @endverbatim
     *
     * Expect from sensor when UI times out:
     * @verbatim
      Stopping user interface
       @endverbatim
     */
    bool startSensor ();

    /**
     * @brief Starts sampling in currently set mode.
     * 
     * @return true  Sampling started successfully.
     * @return false Sampling not started.
     * 
     * Expect from sensor:
     * @verbatim
      VSBA5+0590201.11IRG50137203.08
      CO2-Pro Warmup - W  0.0
       @endverbatim
     * ...
     *
     */
    bool startSampling ();

    /**
     * @brief Stops sampling.
     * 
     * @return true  Sampling stopped successfully.
     * @return false Sampling not stopped.
     * 
     * Expect from sensor: 
     * @verbatim
      Stopping sampling
       @endverbatim
     */
    bool stopSampling ();

    /**
     * @brief Sets the sensor to sample as often as it can (every few seconds).
     * 
     * @param samplesToSkip     Number of continuous mode samples skipped between log entries. Defaults to 0.
     * @param clearZeroCount    Clear the zero count if true. Used only to clear the zero count when replacing the CO2 absorbent. Defaults to false.
     * @param zeroIntervalHours Sets number of hours between zero point corrections (1 to 24h). Defaults to 2h.
     * @return true  Continuous sample mode set successfully.
     * @return false Sensor not set to continuous sample mode.
     * 
     * On entering '3' from main menu, expect:
     * @verbatim
      
      *********************************************************************
      1) Continuous
      2) Timed
      3) Command
      
      Current mode is set to [continuous/timed/command]
      *********************************************************************
      Press ESC to exit
      >>
       @endverbatim
     * On entering '1' from select menu, expect:
     * @verbatim
      
      Continuous mode menu
      ****************************************************************************
       1) Number of continuous mode samples skipped between log entries (0)
       2) Clear the zero count
       3) Re-zero interval in hours                                     (8.00)
      ****************************************************************************
      Press ESC to exit
      >>
       @endverbatim
     * On entering '1' from continuous mode menu, expect:
     * @verbatim
      Enter the number of continuous samples to skip (0 to 1125)>>
       @endverbatim
     * On entering some number [n] and then 0x0d ('\r'), expect:
     * @verbatim
      
      
      Sample skip count: [n]
      
      
      
      Continuous mode menu
      ****************************************************************************
       1) Number of continuous mode samples skipped between log entries ([n])
       2) Clear the zero count
       3) Re-zero interval in hours                                     (8.00)
      ****************************************************************************
      Press ESC to exit
      >>
       @endverbatim
     * On entering '1' from continuous mode menu, expect:
     * @verbatim
      Do you want to reset the zero count (y/n)>> 
       @endverbatim
     * On entering 'y' or 'n', expect:
       @verbatim
      
      Continuous mode menu
      ****************************************************************************
       1) Number of continuous mode samples skipped between log entries [n]
       2) Clear the zero count
       3) Re-zero interval in hours                                     (8.00)
      ****************************************************************************
      Press ESC to exit
      >>
       @endverbatim
     */
    bool setSampleModeContinuous (uint16_t samplesToSkip     = 0,
                                  bool     clearZeroCount    = false,
                                  uint8_t  zeroIntervalHours = 2);
    
    /**
     * @brief Sets the sensor to sample at user-specified intervals.
     * 
     * @param sampleIntervalMins The amount of time between samples in minutes
     *                           (from 1 to 10080 minutes). Defaults to 30min.
     * @param readingsPerSample  The number of samples that are taken at every
     *                           sample interval (from 1 to 20 samples).
     *                           Defaults to 10 samples.
     * @param firstSampleHour    The hour that the sensor will take its first
     *                           sample (24h format). Defaults to 0h.
     * @param firstSampleMin     The minute that the sensor will take its first
     *                           sample (0-60min). Defaults to 0min.
     * @param firstSampleSec     The second that the sensor will take its first
     *                           sample (0-60s). Defaults to 0s.
     * @param logAverage         If greater than zero, sets the number of
     *                           readings to average between per sample.
     *                           Defaults to 0.
     * @param clearZeroCount     Clear the zero count if true. Used only to clear
     *                           the zero count when replacing the CO2 absorbent.
     *                           Defaults to false.
     * @param zeroIntervalHours  Sets number of hours between zero point
     *                           corrections (1 to 24h). Defaults to 2h.
     * @return true  Timed sample mode set successfully.
     * @return false Sensor not set to timed sample mode.
     */
    bool setSampleModeTimed (uint16_t sampleIntervalMins = 30,
                             uint8_t  readingsPerSample  = 10,
                             uint8_t  firstSampleHour    = 0,
                             uint8_t  firstSampleMin     = 0,
                             uint8_t  firstSampleSec     = 0,
                             uint8_t  logAverage         = 0,
                             bool     clearZeroCount     = false,
                             uint8_t  zeroIntervalHours  = 2);
    
    /**
     * @brief Set the sensor to sample when prompted by the user.
     * 
     * @param readingsPerSample The number of samples that are taken at every
     *                          sample interval (from 1 to 20 samples).
     *                          Defaults to 10 samples.
     * @param logAverage        If greater than zero, sets the number of
     *                          readings to average between per sample.
     *                          Defaults to 0.
     * @param clearZeroCount    Clear the zero count if true. Used only to clear
     *                          the zero count when replacing the CO2 absorbent.
     *                          Defaults to false.
     * @param zeroIntervalHours Sets number of hours between zero point
     *                          corrections (1 to 24h). Defaults to 2h.
     * @return true  Command sample mode set successfully.
     * @return false Sensor not set to timed sample mode.
     */ 
    bool setSampleModeCommand (uint8_t  readingsPerSample,
                                  bool     logAverage,
                                  bool     clearZeroCount,
                                  uint8_t  zeroIntervalHours);
    
    /**
     * @brief Triger a sample manually. Sensor must be in sample mode.
     * 
     * @return true  Sample taken successfully.
     * @return false Sample not taken.
     */
    bool commandModeSample ();

    /**
     * @brief View the data logged on the sensor.
     * 
     * Expect from sensor:
     * @verbatim
      Measurement type,Year,Month,Day,Hour,Minute,Second,Zero A/D,Current A/D,CO2,IRGA temperature,Humidity,Humidity sensor temperature,Cell gas pressure,Battery voltage
      %s,%4.0f,%2.0f,%2.0f,%2.0f,%2.0f,%2.0f,%5.0f,%5.0f,%03.02f,%02.1f,%02.2f,%02.2f,%04d,%02.1f
      W M,2020,01,17,00,20,00,55651,51716,532.59,40.00,5.60,0.90,0981,12.1
       @endverbatim
     * 
     * @return true  Logged data viewing started successfully.
     * @return false Logged data viewing not started.
     */
    bool startViewingLoggedData ();
    
    /**
     * @brief Stop viewing the data logged on the sensor.
     * 
     */
    void stopViewingLoggedData ();

    /**
     * @brief Erases data logged on the sensor.
     * 
     * @return true  Data erased successfully.
     * @return false Data not erased.
     */
    bool eraseLoggedData ();
    
    /**
     * @brief Update the status variables.
     * 
     * Updates:
     * -Firmware version.
     * -Current date.
     * -Current time.
     * -Supply voltage (in mV).
     * -Current baud rate.
     * -Current sample mdoe.
     * 
     * @return true  Status updated successfully.
     * @return false Status not updated.
     * 
     * Expect from sensor:
     * @verbatim
      **********************************(CO2 Pro and CO2 Pro CV)**********************************
                                        Oceanus Logger Main Menu
                                            FW Version 1.1.10
                                             Pro Oceanus Inc.
      
      Date: 2018/02/19 Time: 15:58:58
      
      System status:
      Detector ORT = 446.45 hours, Zero count = 95, Supply voltage = 12.0 volts, Mode = continuous
      *******************************************************************************************
       @endverbatim
     */
    bool getStatus ();

    /**
     * @brief Set the sensor's internal clock time, used to timestamp data samples.
     * 
     * @return true  Clock time updated successfully.
     * @return false Clock time not updated.
     */
    bool setClockTime ();
    
    /**
     * @brief Set the baud rate used to communicate with the sensor.
     * 
     * @param baudRate Desired baud rate. Default is 19200.
     * @return true  Baud rate updated successfully.
     * @return false Baud rate not updated.
     */
    bool setBaudRate (uint16_t baudRate = 19200);

    /**
     * @brief Restore sensor settings to factory defaults.
     * 
     * @return true Sensor settings returned to factory default successfully.
     * @return false Sensor settings not returned to factory default.
     */
    bool restoreFactoryDefault ();

    /**
     * @brief Schedule a zero CO2 measurement on the next sample.
     * 
     * Making a measurement on a zero CO2gas sample allows for the infrared detector to maintain a steady baseline value over time to ensure the best accuracy of the data.
     * 
     */
    void scheduleZero (); 
};