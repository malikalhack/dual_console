#include <iostream>
#include <windows.h>

int main() {
    std::cout << "Child console: greetings from the second window!" << std::endl;
    uint8_t sec = 15;
    do {
        std::cout << (uint16_t)sec << " second(s) left   \r";
        Sleep(1000);
    } while (sec--);

    return 0;
}
