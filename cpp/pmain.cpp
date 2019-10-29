#include "EventManager.hpp"
#include <iostream>
#include "FS.hpp"
#include <filesystem>

using namespace std;
string rootPath;

const string NAME_EVENT {"nameEvent"};

void printNameEvent(EventManager::event& event) {
    if (EventManager::getEventName(event) != "nameEvent") return;  // not ours
    cout << "Hey there, " << any_cast<string>(event["name"]) << endl << endl;
}

void inputName() {
    cout << "So your name? "; string name; getline(cin, name);
    EventManager::event event {{EventManager::EVENT_NAME, NAME_EVENT}, {"name", name}};
    EventManager::publishEvent(event);
}

void pmain(vector<string> args) {
    string exePath {filesystem::canonical(filesystem::path(args[0])).parent_path()};
    rootPath = filesystem::canonical(exePath+"/../");

    EventManager::setEventsWaitForListeners();
    inputName(); this_thread::sleep_for(chrono::milliseconds(100));
    EventManager::addEventListener(NAME_EVENT, printNameEvent);

    FS::readTextFile(rootPath+"/cpp/pmain.cpp", [](auto data, auto error){
        if (error=="") cout << "Reading my own source code" << endl << "============================" << endl << data << endl;
        else cout << error << endl;
    });

    FS::readFile(rootPath+"/cpp/pmain.cpp", [](auto data, auto error){
        if (error=="") cout << "Reading my own source code - twice" << endl << "============================" << endl 
            << (char*) &data[0] << endl;
        else cout << error << endl;
    });

    FS::writeTextFile(rootPath+"/tests/test.txt", "Test write\n", [](auto error) {
        cout << "Just wrote something to test.txt" << endl;
    });

    FS::bytes data{}; string s = "Test write 2\n"; for (char c: s) data.push_back((byte)c);
    FS::writeFile(rootPath+"/tests/test2.txt", data, [](auto error) {
        if (error == "") cout << "Just wrote something to test2.txt too" << endl; else cout << error << endl;

        if (error == "") FS::getFileSize(rootPath+"/tests/test2.txt", [](unsigned long size, auto error){
            if (error == "") cout << "Size of test2.txt is = " << size << " bytes" << endl; else cout << error << endl;
        });
    });


    FS::writeTextFile(rootPath+"/tests/dummy.txt", "Dummy file\n", [](auto error) {
        if (error == "") {
            cout << "Just wrote something to dummy.txt, gonna delete it next." << endl;
            FS::deleteFile(rootPath+"/tests/dummy.txt", [](auto error) {
                if (error == "") cout << "And its gone!" << endl; else cout << error << endl;
            });
        } else cout << error << endl; 

        EventManager::shutdownEventLoop();  // we want to exit if this event is processed
    });
}