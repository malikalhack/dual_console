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
    const char* message = "Hello from the server!";
    char buffer[BUF_SIZE];
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
    const char* message = "Hello from process #1!";

    HANDLE hWriteEvent = CreateEventA(nullptr, FALSE, FALSE, writeEventName);
    HANDLE hReadEvent = CreateEventA(nullptr, FALSE, FALSE, readEventName);
    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // Используем файл подкачки
        nullptr,
        PAGE_READWRITE,
        0,
        BUF_SIZE,
        sharedName
    );
    if (!hMap) {
        std::cerr << "Error CreateFileMapping: " << GetLastError() << std::endl;
        return 1;
    }

    LPSTR pBuf = (LPSTR)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
    if (!pBuf) {
        std::cerr << "Error MapViewOfFile: " << GetLastError() << std::endl;
        CloseHandle(hMap);
        return 1;
    }

    CopyMemory(pBuf, message, strlen(message) + 1);
    std::cout << "The message has been written to shared memory.\n";
    SetEvent(hWriteEvent); // сигнализируем, что данные готовы

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

    std::cout << "Message received: " << buffer << std::endl;

    WriteFile(hPipe, message, (DWORD)strlen(message), &bytesWritten, nullptr);
#elif OPTION == 2
    WaitForSingleObject(hReadEvent, INFINITE); // ждём ответ
    std::cout << "Process #1 received: " << pBuf << std::endl;
#endif

    std::cout << "\nMain console: Typing message...";

    // Пишем в текущую консоль
    HANDLE hConsoleMain = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteToConsole(hConsoleMain, 4, "This is the main console.");

    // ждем, пока дочерний процесс закончит работу
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Очистка
#if OPTION == 1
    CloseHandle(hPipe);
#elif OPTION == 2
    UnmapViewOfFile(pBuf);
    CloseHandle(hMap);
#endif
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
