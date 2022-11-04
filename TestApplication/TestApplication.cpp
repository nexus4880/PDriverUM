#include <iostream>
#include <thread>
#include <Windows.h>
#include <string>

int incrementing_number = 0;
bool is_running = true;

int main()
{
    std::cout << "PID (" << GetCurrentProcessId() << ") INREMENTING_NUMBER (" << &incrementing_number << ") IS_RUNNING (" << &is_running << ")\n";
    while (is_running) {
        std::cout << std::string(10,'\b');
        incrementing_number++;
        std::cout << incrementing_number;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "\nEnd" << std::endl;
}
