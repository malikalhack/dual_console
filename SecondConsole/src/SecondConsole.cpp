#include "common.h"

void wait_in_seconds(uint32_t sec = 0) {
    do {
        std::cout << (uint16_t)sec << " second(s) left   \r";
        Sleep(1000);
    } while (sec--);
}

int main() {
#if OPTION == 0
    std::cout << "Child console: greetings from the second window!" << std::endl;
    wait_in_seconds(5);
#elif OPTION == 1
    char buffer[BUF_SIZE];
    const char* message = "Hello from the client!";
    DWORD bytesWritten;
    DWORD bytesRead;

    HANDLE hPipe = CreateFileA(
        pipeName,
        GENERIC_WRITE | GENERIC_READ,
        0, nullptr,
        OPEN_EXISTING,
        0, nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to the channel.\n";
        return 1;
    }

    WriteFile(hPipe, message, (DWORD)strlen(message), &bytesWritten, nullptr);

    ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
    buffer[bytesRead] = '\0';
    std::cout << "Message received: " << buffer << std::endl;

    CloseHandle(hPipe);
    wait_in_seconds(10);
#elif OPTION == 2
    const char* message = "Reply from process #2!";

    HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, sharedName);
    LPSTR pBuf = (LPSTR)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

    HANDLE hWriteEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, writeEventName);
    HANDLE hReadEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, readEventName);

    WaitForSingleObject(hWriteEvent, INFINITE); // ждём, пока процесс 1 запишет
    std::cout << "Process #2 received: " << pBuf << std::endl;

    CopyMemory(pBuf, message, strlen(message) + 1);
    SetEvent(hReadEvent); // сигнализируем, что ответ готов

    UnmapViewOfFile(pBuf);
    CloseHandle(hMap);
    wait_in_seconds(10);
#else
#error Unexpected option
#endif
    return 0;
}
