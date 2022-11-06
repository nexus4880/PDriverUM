#include <iostream>
#include <thread>
#include <Windows.h>
#include <string>

int incrementing_number = 0;
bool is_running = true;

struct Vector3 {
    float x, y, z;
};

Vector3 vec = { 1.5f, 4.f, 6.7f };

int main()
{
    const int pid = GetCurrentProcessId();
    while (is_running) {
        incrementing_number++;
        printf_s("PID (%i) | INCREMENTING_NUMBER (%i | %p) | IS_RUNNING (%p) | VEC (%f, %f, %f | %p)\n", pid, incrementing_number, &incrementing_number, &is_running, vec.x, vec.y, vec.z, &vec);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\nEnd" << std::endl;
}
