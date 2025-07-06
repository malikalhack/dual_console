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
    wait_in_seconds(15);
#elif OPTION == 1
    const char* pipeName = R"(\\.\pipe\MyPipe)";
    const char* message = "Hello from the client!";
    DWORD bytesWritten;

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

    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
    buffer[bytesRead] = '\0';
    std::cout << "Message received: " << buffer << "\n";

    CloseHandle(hPipe);
    wait_in_seconds(10);
#elif OPTION == 2

#else
#error Unexpected option
#endif
    return 0;
}
