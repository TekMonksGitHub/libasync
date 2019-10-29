#include "AsyncIO.hpp"
#include "EventManager.hpp"
#include "FS.hpp"
#include <thread>
#include <vector>

using namespace std;

namespace AsyncIO {
    const string ASYNC_IO_EVENT_NAME = "com_tekmonks_cpp_eventframework_event_asyncio";
    const string ASYNC_IO_EVENT_TYPE = "com_tekmonks_cpp_eventframework_event_asyncio_type";

    // forward declaration for non-exported functions and members below
    vector<thread> _threads;
    auto _dontStop {false};
    void _doTick();
    void _processAsyncEvent(EventManager::event);
    void _sendTextData(const pair<string,bool>, EventManager::event&);
    void _sendBinaryData(const pair<FS::bytes,bool>, EventManager::event&);
    void _sendSimpleResultEvent(bool, EventManager::event&);
    void _sendULongResultEvent(unsigned long, EventManager::event&);
    void _createReplyEvent(bool, const string, const string, any, EventManager::event&);

    void start() {
        _dontStop = true;
        for (auto i = 0; i < thread::hardware_concurrency(); i++) _threads.push_back(thread {_doTick});
    }

    void stop() {
        _dontStop = false;
        for (auto i = 0; i < thread::hardware_concurrency(); i++) _threads[i].join();   // wait for all to finish
    }

    void _doTick() {
        while (_dontStop) {
            const auto eventThis = EventManager::getEventForEventName(ASYNC_IO_EVENT_NAME);
            if (!EventManager::isNullEvent(eventThis)) _processAsyncEvent(eventThis);
        }
    }

    void _processAsyncEvent(EventManager::event event) {
        auto eventType = any_cast<string>(event[ASYNC_IO_EVENT_TYPE]);
        
        if (eventType == FS::OP_READ_TEXT_FILE) _sendTextData(FS::readTextFileSync(any_cast<string>(event["path"])), event);
        if (eventType == FS::OP_READ_FILE) _sendBinaryData(FS::readFileSync(any_cast<string>(event["path"]), 
            event.count("bytesToRead")?any_cast<int>(event["bytesToRead"]):-1), event);
        if (eventType == FS::OP_WRITE_TEXT_FILE) _sendSimpleResultEvent(FS::writeTextFileSync(any_cast<string>(event["path"]), 
            any_cast<string>(event["data"]), event.count("overwrite") ? any_cast<bool>(event["overwrite"]) : true), event);
        if (eventType == FS::OP_WRITE_FILE) _sendSimpleResultEvent(FS::writeFileSync(any_cast<string>(event["path"]), 
            any_cast<FS::bytes>(event["data"]), event.count("overwrite") ? any_cast<bool>(event["overwrite"]) : true), event);
        if (eventType == FS::OP_DELETE_FILE) _sendSimpleResultEvent(FS::deleteFileSync(any_cast<string>(event["path"])), event);
        if (eventType == FS::OP_FILE_SIZE) _sendULongResultEvent(FS::getFileSizeSync(any_cast<string>(event["path"])), event);
    }

    void _sendTextData(const pair<string,bool> data, EventManager::event& event) {
        _createReplyEvent(data.second, "File read error", "data", data.first, event);
        EventManager::publishEvent(event);
    }

    void _sendULongResultEvent(unsigned long result, EventManager::event& event) {
        _createReplyEvent(result == -1, "File system error", "result", result, event);
        EventManager::publishEvent(event);
    }

    void _sendBinaryData(const pair<FS::bytes,bool> data, EventManager::event& event) {
        _createReplyEvent(data.second, "File read error", "data", data.first, event);
        EventManager::publishEvent(event);
    }

    void _sendSimpleResultEvent(bool result, EventManager::event& event) {
        _createReplyEvent(!result, "IO error", "", "", event);
        EventManager::publishEvent(event);
    }

    void _createReplyEvent(bool conditionError, const string errorMsg, const string resultName, any result, EventManager::event& event) {
        event[EventManager::EVENT_NAME] = event["reply_name"];
        if (conditionError) {
            event[EventManager::ERROR_EVENT_FLAG] = EventManager::ERROR_EVENT_FLAG;
            event["error"] = errorMsg;
        } else if (resultName != "") event[resultName] = result;
    }
}