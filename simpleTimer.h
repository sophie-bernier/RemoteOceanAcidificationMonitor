/**
 * @file simpleTimer.h
 * @author Sophie Bernier (sophie.liz.ber@gmail.com)
 * @brief Header file for the simpleTimer class and its dependent enums, defaults, et cetera.
 * @version 0.0.1
 * @date 2021-06-29
 * 
 * @warning Under heavy development. Use at your own risk.
 * 
 */

#include <Arduino.h>

/**
 * @brief Simple timer class. Uses millis() to call a function and set a flag on timeout.
 * 
 */
class simpleTimer
{
  public:
    /**
     * @brief Constructs a new simple Timer object
     * 
     * @param _timerCallback Pointer to function to call when the timer times out.
     * @param _timeoutMillis Milliseconds to time.
     * @param _currentMillis Reference to the variable which is compared to determine if time has elapsed.
     * @param _repeating     Whether the timer repeats after timing out or not.
     */
    simpleTimer(void (* _timerCallback) (),
                uint32_t const _timeoutMillis,
                uint32_t const & _currentMillis,
                bool const _repeating):
                timerCallback{_timerCallback},
                timeoutMillis{_timeoutMillis},
                currentMillis{_currentMillis},
                repeating{_repeating} {}
    /**
     * @brief Constructs a new simple Timer object
     * 
     * @param _timeoutMillis Milliseconds to time.
     * @param _currentMillis Reference to the time which is referenced.
     * @param _repeating     Whether the timer repeats after timing out or not.
     * 
     * @overload
     */
    simpleTimer (uint32_t const _timeoutMillis,
                 uint32_t const & _currentMillis,
                 bool const _repeating
                 ):
                   timeoutMillis{_timeoutMillis},
                   currentMillis{_currentMillis},
                   repeating{_repeating}
                   {
                     
                   }
    /**
     * @brief Stops the timer. The timer automatically resets if updated while stopped.
     * 
     * @todo Have the timer keep its elapsed time if updated while stopped.
     */
    void pause ()
    {
      started = false;
    }

    /**
     * @brief Starts the timer.
     * 
     */
    void start ()
    {
      started = true;
    }

    void setTimeout (uint32_t _timeoutMillis)
    {
      timeoutMillis = _timeoutMillis;
    }

    /**
     * @brief Resets the timer to zero elapsed time.
     * 
     */
    void reset ()
    {
      referenceMillis = currentMillis;
    }

    /**
     * @brief Clears the 'done' flag.
     * 
     */
    void clearDone ()
    {
      done = false;
    }

    /**
     * @brief Checks if the timer has elapsed.
     * 
     * If so, sets the 'done' flag and call the callback function if it is initialized.
     * 
     */
    void update ()
    {
      //Serial.print("cur: ");
      //Serial.println(currentMillis);
      //Serial.print("ref: ");
      //Serial.println(referenceMillis);
      if (started)
      {
        /*
        Serial.print("currentMillis: ");
        Serial.println(currentMillis);
        Serial.print("referenceMillis: ");
        Serial.println(referenceMillis);
        Serial.print("cur-ref: ");
        Serial.println(currentMillis - referenceMillis);
        */
        if ((currentMillis - referenceMillis) > timeoutMillis)
        {
          done = true;
          if (timerCallback != NULL)
          {
            timerCallback();
          }
          reset();
          if (!repeating)
          {
            pause();
          }
        }
      }
      else
      {
        // Option 1: Keep advancing referenceMillis so that timer does not advance.
        //referenceMillis += currentMillis - referenceMillis;
        // Option 2: Reset timer on stop.
        referenceMillis = currentMillis;
      }
    }
    
    /**
     * @brief Checks if the timer is running.
     * 
     * @return true  Timer is started.
     * @return false Timer is paused.
     */
    bool isRunning ()
    {
      return started;
    }

    /**
     * @brief Checks the done flag.
     * 
     * @return true  Timer **has** timed out since 'done' flag was reset.
     * @return false Timer has **not** timed out since 'done' flag was reset.
     */
    bool isDone ()
    {
      return done;
    }
  private:
    void (* timerCallback) () = NULL;
    uint32_t timeoutMillis = 0xFFFFFFFF;
    uint32_t const & currentMillis;
    uint32_t referenceMillis = 0;
    bool repeating = false;
    bool started = false;
    bool done = false;
};

/**
 * @brief Class to handle multiple timers at once.
 * @deprecated 
 * 
 */
/*
class simpleTimerService
{
  public:
    void tick ();
    void addTimer (simpleTimer * timerToAdd);
  protected:
    uint32_t currentMillis = 0;
    std::list<simpleTimer*> simpleTimerList;
};
*/