#include <iostream>
#include <thread>
#include <Windows.h>

int random_number;

int main()
{
    srand(time(nullptr));
    int test = 10;
    std::cout << GetCurrentProcessId() << std::endl;
    while (true) {
        random_number = rand();
        std::cout << "Current number is: " << random_number << ", address is: " << &random_number << std::endl;
        std::cout << test << std::endl;
        std::cout << &test << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}
