#include <string>
#include <thread>
#include "common.h"

void WriteToConsole(HANDLE hConsole, int y, const std::string& message) {
    COORD coord = { 0, static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, coord);
    DWORD written;
    WriteConsoleA(hConsole, message.c_str(), (DWORD)message.size(), &written, nullptr);
}

int main() {
#if OPTION == 1
    const char* pipeName = R"(\\.\pipe\MyPipe)";
    const char* message = "Hello from the server!";
    char buffer[128];
    DWORD bytesRead;
    DWORD bytesWritten;

    HANDLE hPipe = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_DUPLEX, //PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1, 0, 0, 0, nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating channel.\n";
        return 1;
    }
#elif OPTION == 2

#endif

    // Создаем вторую консоль (новый процесс)
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Путь к исполняемому файлу (запускаем копию этой же программы)
    const char* cmdLine = "SecondConsole.exe";

    if (!CreateProcessA(
        nullptr,
        (LPSTR)cmdLine,
        nullptr, nullptr,
        FALSE,
        CREATE_NEW_CONSOLE, // создаем в новой консоли!
        nullptr,
        nullptr,
        &si,
        &pi
    )) {
        std::cerr << "Failed to create second console.\n";
        return 1;
    }

#if OPTION == 1
    std::cout << "Waiting for client connection...\n";
    ConnectNamedPipe(hPipe, nullptr);

    ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
    buffer[bytesRead] = '\0';

    std::cout << "Message received: " << buffer << "\n";

    WriteFile(hPipe, message, (DWORD)strlen(message), &bytesWritten, nullptr);
#endif

    std::cout << "Main console: Typing message...\n";

    // Пишем в текущую консоль
    HANDLE hConsoleMain = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteToConsole(hConsoleMain, 3, "This is the main console.");

    // ждем, пока дочерний процесс закончит работу
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Очистка
#if OPTION == 1
    CloseHandle(hPipe);
#endif
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
