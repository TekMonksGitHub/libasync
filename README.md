# libasync
Cross platform Async Library for C++

Coded to comply with C++17 and C++20 standards (https://en.wikipedia.org/wiki/C%2B%2B20). STD classes, no Boost, no Pointers.

![](https://tekmonks.com/images/libasync.jpeg)

## Why
No comprehensive Async IO library exists for C++. Many exist for Async tasks, or Sockets, but nothing that covers everything. This library will cover

* Async File IO
* Async CPU Intensive IO
* Async Network IO
* Async Extensions

## Why C++20 and No Pointers
Modern C++ is a beautiful systems language. Our aim is to produce a C++ library coded for the future, with no legacy code or issues. 

Pointers are avoided due to massive security issues they cause, and problems like Buffer Overflows and Code-Injection problems. For example,
* https://www.vice.com/en_us/article/a3mgxb/the-internet-has-a-huge-cc-problem-and-developers-dont-want-to-deal-with-it
* http://www.cs.kuleuven.be/publicaties/rapporten/cw/CW386.pdf
* https://www.theregister.co.uk/2019/07/18/microsoft_rust_security/

Our plan is to avoid pointers all-together. Not just, move to smart pointers. `libasync` is also an experiment to produce a modern C++ runtime library which avoids pointers completely, if possible. 

## How to use
Step 1: Download code  
Step 2: Download [XForge](https://github.com/TekMonksGitHub/xforge). This is needed to build.  
Step 3: `xforge -c -f [path]/build/build.xf.js -o main`  
Step 4: Doxygen docs are in `[path]/docs`  
Step 5: Look at `[path]/cpp/pmain.cpp` for an example  
    
## Want to contribute? Why this exists?
Our goal is to produce a simple and comprehensive C++ async library to make it easier to write

* High performace TCP and UDP servers
* High performance File IO servers
* High performance APIs in C++ 

Sometimes you just need the raw performance of C++ ;)
