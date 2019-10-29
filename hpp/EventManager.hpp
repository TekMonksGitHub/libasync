#pragma once

#include <map>
#include <any>
#include <queue>
#include <string>
#include <vector>
#include <thread>

using namespace std;

namespace EventManager {
    using event = map<string, any>;
    using eventlistener = void (*)(event&);
    using eventlisteners = vector<eventlistener>;
    using eventQueue = vector<event>;

    const string EVENT_NAME = "com_tekmonks_cpp_eventframework_eventmanager_event";
    const string ERROR_EVENT_FLAG = "com_tekmonks_cpp_eventframework_eventmanager_error_event";
    extern const event NULL_EVENT;
    extern const event ERROR_EVENT;

    void publishEvent(const event&);

    void addEventListener(const string eventName, const eventlistener);

    void removeEventListener(const string eventName, const eventlistener);

    const string getEventName(const event&);

    bool areTherePendingEvents();

    void start(); 

    void stop();

    void setEventsWaitForListeners(bool wait=true);

    const event getEventForEventName(string eventName);

    bool isNullEvent(const event&);

    bool isErrorEvent(const event&);

    const string getError(const event&);

    bool isEventLoopShutdownRequested();

    int getProgramExitCode();

    void shutdownEventLoop(int=0);
}