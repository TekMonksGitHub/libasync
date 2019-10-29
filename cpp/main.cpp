#include <vector>
#include "EventManager.hpp"
#include "AsyncIO.hpp"

int main(int argc, char* argv[]) {
    vector<string> args {};
    for (auto i = 0; i < argc; i++) args.push_back(argv[i]);

    EventManager::start();                                      // start the event loop
    AsyncIO::start();                                           // start Async IO
    void pmain(vector<string>); pmain(args);                    // run the main program
    while (!EventManager::isEventLoopShutdownRequested());       // program requested shutdown
    AsyncIO::stop();                                            // stop Async IO
    while (EventManager::areTherePendingEvents());              // wait for all events to be processed
    EventManager::stop();                                       // stop the event loop
    return EventManager::getProgramExitCode();                  // whatever the main code returned
}