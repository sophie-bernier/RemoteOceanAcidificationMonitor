// simpleTimer.h

#include <Arduino.h>

class simpleTimer
{
  public:
    simpleTimer(void (* _timerCallback) (),
                uint32_t const _timeoutMillis,
                uint32_t const & _currentMillis,
                bool const _repeating):
                timerCallback{_timerCallback},
                timeoutMillis{_timeoutMillis},
                currentMillis{_currentMillis},
                repeating{_repeating} {}
                
    simpleTimer (uint32_t const _timeoutMillis,
                 uint32_t const & _currentMillis,
                 bool const _repeating
                 ):
                   timeoutMillis{_timeoutMillis},
                   currentMillis{_currentMillis},
                   repeating{_repeating}
                   {
                     
                   }
    void pause ()
    {
      started = false;
    }
    void start ()
    {
      started = true;
    }
    void reset ()
    {
      referenceMillis = currentMillis;
    }
    void clearDone ()
    {
      done = false;
    }
    void update ()
    {
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
    bool isRunning ()
    {
      return started;
    }
    bool isDone ()
    {
      return done;
    }
  private:
    void (* timerCallback) () = NULL;
    uint32_t const timeoutMillis = 0xFFFFFFFF;
    uint32_t const & currentMillis = 0;
    uint32_t referenceMillis = 0;
    bool repeating = false;
    bool started = false;
    bool done = false;
};
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