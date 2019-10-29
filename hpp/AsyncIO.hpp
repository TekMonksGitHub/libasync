#pragma once

#include <string>

using namespace std;

namespace AsyncIO {
    extern const string ASYNC_IO_EVENT_NAME;
    extern const string ASYNC_IO_EVENT_TYPE;

    void start();
    void stop();
}