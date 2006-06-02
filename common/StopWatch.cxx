#include "StopWatch.h"

#include <stdio.h>

#include "Logger.h"

StopWatch::StopWatch(void)
{
    this->events = new std::vector<TimeEvent*>;
    this->started = false;
}

StopWatch::~StopWatch(void)
{
}

void StopWatch::ShowTime()
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char message[80];
    sprintf(message, "Time => %s", asctime(timeinfo));
    Logger::logInfo(message);
}

void StopWatch::Start(std::string label)
{
    if (this->started)
        return;

    this->events->push_back(new TimeEvent(label));
    this->started = true;
}

void StopWatch::Lap(std::string label)
{
    if (!this->started)
        return;

    this->events->push_back(new TimeEvent(label));
}

void StopWatch::Stop(std::string label)
{
    if (!this->started)
        return;

    this->events->push_back(new TimeEvent(label));
    this->started = false;
}

double StopWatch::ElapsedTime(TimeEvent* pEvent)
{
    return (*pEvent - **(this->events->begin()));
}

double StopWatch::ElapsedTime()
{
    return this->ElapsedTime(new TimeEvent());
}

void StopWatch::Print()
{
    std::vector<TimeEvent*>::iterator eventIt = this->events->begin();

    // Print out first event
    TimeEvent* start = *eventIt;
    std::string message;
    message.append(start->label).append(":\t").append(ctime(&(start->eTime)));
    // sprintf(message, "%s:\t%s", start->label, ctime(&start->eTime));
    Logger::logInfo(message);
    // std::cout << start->label << ":\t" << ctime(&start->eTime) << std::endl;

    for (*eventIt++ ; eventIt != this->events->end(); *eventIt++)
    {
        message = "";
        message.append((*eventIt)->label).append(":\t").append(StopWatch::HMS(this->ElapsedTime(*eventIt)));
        // sprintf(message, "%s:\t%s", (*eventIt)->label, StopWatch::HMS(this->ElapsedTime(*eventIt)));
        Logger::logInfo(message);
        // std::cout << (*eventIt)->label << ":\t" << StopWatch::HMS(this->ElapsedTime(*eventIt)) << std::endl;
    }
}

std::string StopWatch::HMS(double secs)
{
    int hours = secs / 3600;
    double remain = secs - (3600 * hours);
    int mins = remain / 60;
    remain = remain - (60 * mins);

    char buffer[50];
    sprintf(buffer, "%d:%d:%f", hours, mins, remain);
    std::string str(buffer);
    return str;
}

TimeEvent::TimeEvent(std::string label)
{
    this->label = label;
    this->eTime = time(NULL);
}

TimeEvent::TimeEvent(void)
{
    this->label = "";
    this->eTime = time(NULL);
}

TimeEvent::~TimeEvent()
{
}

double TimeEvent::operator -(TimeEvent event)
{
    return difftime(this->eTime, event.eTime);
}
