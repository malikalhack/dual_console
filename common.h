#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <iostream>

// 0 - Timer
// 1 - Named pipe
// 2 - Shared Memory

#define OPTION  (2)

const unsigned int BUF_SIZE = 256;

#if OPTION == 1
const char* pipeName = R"(\\.\pipe\MyPipe)";
#elif OPTION == 2
const char* sharedName = "Global\\MySharedMemory";
const char* writeEventName = "writeEvent";
const char* readEventName = "readEvent";
#endif

#endif // !COMMON_H