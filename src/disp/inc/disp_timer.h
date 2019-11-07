#ifndef _DISP_TIMER_H_
#define _DISP_TIMER_H_

#include "os_type.h"

//The application time based timer
class DispTimer {
public:
    //Initializes variables
    DispTimer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    UINT32 getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    //The clock time when the timer started
    UINT32 mStartTicks;

    //The ticks stored when the timer was paused
    UINT32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};



#endif

