#include "FS.hpp"
#include "Util.hpp"
#include "AsyncIO.hpp"
#include "EventManager.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

using namespace std;

namespace FS {
    const string OP_READ_TEXT_FILE = "FS::readTextFile";
    const string OP_READ_FILE = "FS::readFile";
    const string OP_WRITE_TEXT_FILE = "FS::writeTextFile";
    const string OP_WRITE_FILE = "FS::writeFile";
    const string OP_DELETE_FILE = "FS::deleteFile";
    const string OP_FILE_SIZE = "FS::fileSize";

    void readTextFile(const string path, callbackStr cb) {
        string uuid = Util::get_uuid();
        EventManager::event eventThis {
            {EventManager::EVENT_NAME, AsyncIO::ASYNC_IO_EVENT_NAME},
            {AsyncIO::ASYNC_IO_EVENT_TYPE, OP_READ_TEXT_FILE},
            {"path", path},
            {"reply_name", uuid},
            {"cb", cb}
        };
        EventManager::publishEvent(eventThis);
        EventManager::addEventListener(uuid, [](EventManager::event& reply){
            callbackStr cb = any_cast<callbackStr>(reply["cb"]);
            if (EventManager::isErrorEvent(reply)) cb("", EventManager::getError(reply));
            else cb(any_cast<string>(reply["data"]), "");
        });
    }

    const pair<string,bool> readTextFileSync(const string path) {
        ifstream inFile(path, ios::in);
        if (!inFile.is_open()) return pair<string,bool>{"", true};

        string data; inFile.seekg(0, std::ios::end); data.resize(inFile.tellg()); inFile.seekg(0, std::ios::beg);
        inFile.read(&data[0], data.size()); inFile.close();
        return pair<string,bool>{data, false};
    }

    void readFile(const string path, callbackBin cb, int bytesToRead) {
        string uuid = Util::get_uuid();
        EventManager::event eventThis {
            {EventManager::EVENT_NAME, AsyncIO::ASYNC_IO_EVENT_NAME},
            {AsyncIO::ASYNC_IO_EVENT_TYPE, OP_READ_FILE},
            {"path", path},
            {"reply_name", uuid},
            {"cb", cb}
        };
        EventManager::publishEvent(eventThis);
        EventManager::addEventListener(uuid, [](EventManager::event& reply){
            callbackBin cb = any_cast<callbackBin>(reply["cb"]);
            if (EventManager::isErrorEvent(reply)) cb({}, EventManager::getError(reply));
            else cb(any_cast<bytes>(reply["data"]), "");
        });
    }

    const pair<bytes,bool> readFileSync(const string path, int bytesToRead) {
        ifstream inFile(path, ios::in | ios::binary);
        if (!inFile.is_open()) return pair<bytes,bool>{{}, true};

        bytes data; 
        if (bytesToRead == -1) {inFile.seekg(0, std::ios::end); bytesToRead = inFile.tellg(); inFile.seekg(0, std::ios::beg);}
        data.resize(bytesToRead);

        char acData[bytesToRead];
        inFile.read(acData, bytesToRead); inFile.close();

        for (int i = 0; i< bytesToRead; i++) data[i] = (byte)acData[i];
        return pair<bytes,bool>{data, false};
    }

    void writeFile(const string path, const bytes data, callbackDone cb, const bool overwrite) {
        string uuid = Util::get_uuid();
        EventManager::event eventThis {
            {EventManager::EVENT_NAME, AsyncIO::ASYNC_IO_EVENT_NAME},
            {AsyncIO::ASYNC_IO_EVENT_TYPE, OP_WRITE_FILE},
            {"path", path},
            {"reply_name", uuid},
            {"cb", cb},
            {"data", data}
        };
        EventManager::publishEvent(eventThis);
        EventManager::addEventListener(uuid, [](EventManager::event& reply){
            callbackDone cb = any_cast<callbackDone>(reply["cb"]);
            if (EventManager::isErrorEvent(reply)) cb(EventManager::getError(reply));
            else cb("");
        });
    }

    bool writeFileSync(const string path, const bytes data, const bool overwrite) {
        ofstream outFile(path, ios::out | overwrite? ios::trunc : ios::app);
        if (!outFile.is_open()) return false;

        string sData; sData.resize(data.size()); for (byte c: data) sData += (unsigned char)c;
        outFile.write(sData.c_str(), sData.size()); outFile.close(); return true;
    }

    void writeTextFile(const string path, const string data, callbackDone cb, const bool overwrite) {
        bytes byteData{}; for (char c: data) byteData.push_back((byte)c);
        writeFile(path, byteData, cb, overwrite);
    }

    bool writeTextFileSync(const string path, const string data, const bool overwrite) {
        bytes byteData{}; for (char c: data) byteData.push_back((byte)c);
        return writeFileSync(path, byteData, overwrite);
    }

    void deleteFile(const string path, callbackDone cb) {
        string uuid = Util::get_uuid();
        EventManager::event eventThis {
            {EventManager::EVENT_NAME, AsyncIO::ASYNC_IO_EVENT_NAME},
            {AsyncIO::ASYNC_IO_EVENT_TYPE, OP_DELETE_FILE},
            {"path", path},
            {"reply_name", uuid},
            {"cb", cb}
        };
        EventManager::publishEvent(eventThis);
        EventManager::addEventListener(uuid, [](EventManager::event& reply){
            callbackDone cb = any_cast<callbackDone>(reply["cb"]);
            if (EventManager::isErrorEvent(reply)) cb(EventManager::getError(reply));
            else cb("");
        });
    }

    bool deleteFileSync(const string path) {
        return remove(path.c_str()) == 0 ? true : false;
    }

    void getFileSize(const string path, callbackULong cb) {
        string uuid = Util::get_uuid();
        EventManager::event eventThis {
            {EventManager::EVENT_NAME, AsyncIO::ASYNC_IO_EVENT_NAME},
            {AsyncIO::ASYNC_IO_EVENT_TYPE, OP_FILE_SIZE},
            {"path", path},
            {"reply_name", uuid},
            {"cb", cb}
        };
        EventManager::publishEvent(eventThis);
        EventManager::addEventListener(uuid, [](EventManager::event& reply){
            callbackULong cb = any_cast<callbackULong>(reply["cb"]);
            if (EventManager::isErrorEvent(reply)) cb(-1, EventManager::getError(reply));
            else cb(any_cast<unsigned long>(reply["result"]), "");
        });
    }

    unsigned long getFileSizeSync(const string path) {
        return filesystem::file_size(path);
    }
}