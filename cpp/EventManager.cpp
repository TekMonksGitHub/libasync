#include "EventManager.hpp"
#include "AsyncIO.hpp"

using namespace std;

namespace EventManager {
    eventQueue _queueEvents;
    mutex _mutex_eventqueue; 
    map<string, eventlisteners> _listeners;
    mutex _mutex_listeners;
    auto _eventLoopShutdown {false};
    auto _programExitCode {-1};
    auto _dontStop {true};
    thread _threadEventLoop;
    auto _eventsWaitForListeners {false};
    const event NULL_EVENT {{EVENT_NAME, string{"com_tekmonks_cpp_eventframework_eventmanager_event_null_event"}}};
    const event ERROR_EVENT {{ERROR_EVENT_FLAG, ERROR_EVENT_FLAG}, {"error", ""}};

    // forward declaration for non-exported functions below
    void _doTick();
    void _sendFrontQueueEventToListeners();

    void publishEvent(const event& newEvent) {
        lock_guard<mutex> lock(_mutex_eventqueue); _queueEvents.push_back(newEvent); 
    }

    void addEventListener(const string eventName, const eventlistener listenerThis) {
        lock_guard<mutex> lock(_mutex_listeners);
        if (_listeners.find(eventName) == _listeners.end()) _listeners[eventName] = move(eventlisteners {listenerThis});
        else _listeners[eventName].push_back(listenerThis);
    }

    void removeEventListener(const string eventName, const eventlistener listenerThis) {
        if (_listeners.find(eventName) != _listeners.end()) {
            lock_guard<mutex> lock{_mutex_listeners};
            auto location = find(_listeners[eventName].begin(), _listeners[eventName].end(), listenerThis);
            if (location != _listeners[eventName].end()) _listeners[eventName].erase(location);
        }
    }

    const event getEventForEventName(const string eventName) {
        lock_guard<mutex> lock(_mutex_eventqueue);
        auto pos {-1}; 
        for (auto index = 0; index < _queueEvents.size(); index++)
            if (any_cast<string>(_queueEvents[index][EVENT_NAME]) == eventName) pos = index;
        
        if (pos != -1) {
            event eventThis = move(_queueEvents[pos]); 
            _queueEvents.erase(_queueEvents.begin()+pos); 
            return eventThis;
        } 
        else return NULL_EVENT;
    }

    void start() {
        _dontStop = true;
        _threadEventLoop = move(thread {_doTick});
    }

    void stop() {
        _dontStop = false;
        _threadEventLoop.join();
    }

    bool isEventLoopShutdownRequested() {
        return _eventLoopShutdown;
    }

    int getProgramExitCode() {
        return _programExitCode;
    }

    void shutdownEventLoop(int returnCode) {
        _eventLoopShutdown = true;
        _programExitCode = returnCode;
    }

    bool areTherePendingEvents() {
        lock_guard<mutex> lock(_mutex_eventqueue);
        return !_queueEvents.empty();
    }

    const string getEventName(const event& eventThis) {
        return any_cast<string>(eventThis.at(EVENT_NAME));
    }

    void setEventsWaitForListeners(bool wait) {
        _eventsWaitForListeners = wait;
    }

    bool isNullEvent(const event& event) {
        return any_cast<string>(event.at(EVENT_NAME)) == "com_tekmonks_cpp_eventframework_eventmanager_event_null_event";
    }

    bool isErrorEvent(const event& event) {
        return event.count(ERROR_EVENT_FLAG) > 0;
    }

    const string getError(const event& event) {
        return any_cast<string>(event.at("error"));
    }

    void _doTick() {
        while (_dontStop) _sendFrontQueueEventToListeners();
    }

    void _sendFrontQueueEventToListeners() {
        unique_lock<mutex> lock{_mutex_eventqueue}; 
        if (_queueEvents.empty()) {lock.unlock(); return;}              // nothing to do
        
        auto front = move(_queueEvents.front()); _queueEvents.erase(_queueEvents.begin()); 
        lock.unlock();

        const auto eventName = any_cast<string>(front[EVENT_NAME]);
        if (_listeners.find(eventName) != _listeners.end()) {   // send to listeners and now its gone forever
            auto& listeners_this_event = _listeners[eventName];
            for (eventlistener listenerThis: listeners_this_event) listenerThis(front);
        } else if (_eventsWaitForListeners) publishEvent(front); // if we need to wait for listeners, push it to the back
    }
}
