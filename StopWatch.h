#pragma once
#include <time.h>
#include <string>
#include <vector>

class TimeEvent
{
public:
    TimeEvent(void);
    TimeEvent(std::string label);
    ~TimeEvent(void);
    double operator-(TimeEvent event);

    std::string label;
    time_t eTime;
};

class StopWatch
{
public:
    StopWatch(void);
    ~StopWatch(void);

    static void ShowTime(void);
    static std::string HMS(double secs);

    void Start(std::string label);
    void Lap(std::string label);
    void Stop(std::string label);
    double ElapsedTime(void);
    double ElapsedTime(TimeEvent* pEvent);
    void Print(void);
private:
    std::vector<TimeEvent*>* events;
    bool started;
};
