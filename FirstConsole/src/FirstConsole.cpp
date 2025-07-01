#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

void WriteToConsole(HANDLE hConsole, int y, const std::string& message) {
    COORD coord = { 0, static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, coord);
    DWORD written;
    WriteConsoleA(hConsole, message.c_str(), message.size(), &written, nullptr);
}

int main() {
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

    std::cout << "Main console: Typing message...\n";

    // Пишем в текущую консоль
    HANDLE hConsoleMain = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteToConsole(hConsoleMain, 3, "This is the main console.");

    // Пример — ждем, пока дочерний процесс закончит работу
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Очистка
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
