#include "TestStopWatch.h"

#include <time.h>
#include <iostream>

#include "..\StopWatch.h"

TestStopWatch::TestStopWatch(void)
{
}

TestStopWatch::~TestStopWatch(void)
{
}

void TestStopWatch::run()
{
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
        label.append(itoa(i, "", 10));
        watch->Lap(label);
    }
    std::cout << "Elapsed: " << watch->ElapsedTime() << std::endl;
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