#include "TestStopWatch.h"

#include <time.h>
#include <string>

#include "..\Logger.h"
#include "..\StopWatch.h"

TestStopWatch::TestStopWatch(void)
{
}

TestStopWatch::~TestStopWatch(void)
{
}

void TestStopWatch::run()
{
    Logger::logInfo("Running TestStopWatch");
    this->testStartStop();
}

void TestStopWatch::testStartStop()
{
    StopWatch* watch = new StopWatch();
    std::string label;
    watch->Start("test start");
    for (int i = 0; i < 10; i++)
    {
        this->sleep(1);
        label = "test lap ";
        label.append(itoa(i, new char[5], 10));
        watch->Lap(label);
    }
    char message[80];
    sprintf(message, "Elapsed: %5.4f", watch->ElapsedTime());
    Logger::logDebug(message);
    watch->Stop("test stop");
    watch->Print();
    succeed_();
}

void TestStopWatch::sleep(long secs)
{
    time_t start_time, cur_time;

    time(&start_time);
    do
    {
            time(&cur_time);
    }
    while((cur_time - start_time) < secs);
}