#pragma once

#include <string>
#include <iostream>

using namespace std;

namespace FS {
    using error = string;
    using bytes = vector<byte>;

    using callbackStr = void(*)(string, error);
    using callbackULong = void(*)(unsigned long, error);
    using callbackBin = void(*)(bytes, error);
    using callbackDone = void(*)(error);

    void readTextFile(const string, callbackStr);
    const pair<string,bool> readTextFileSync(const string);
    void readFile(const string, callbackBin, int=-1);
    const pair<bytes,bool> readFileSync(const string, int=-1);

    void writeTextFile(const string, const string, callbackDone, const bool=true);
    bool writeTextFileSync(const string, const string, const bool=true);
    void writeFile(const string, const bytes, callbackDone, const bool=true);
    bool writeFileSync(const string, const bytes, const bool=true);

    void deleteFile(const string, callbackDone);
    bool deleteFileSync(const string);

    void getFileSize(const string, callbackULong);
    unsigned long getFileSizeSync(const string);

    extern const string OP_READ_TEXT_FILE;
    extern const string OP_READ_FILE;
    extern const string OP_WRITE_TEXT_FILE;
    extern const string OP_WRITE_FILE;
    extern const string OP_DELETE_FILE;
    extern const string OP_FILE_SIZE;
}