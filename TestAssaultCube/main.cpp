#include <iostream>
#include <thread>
#include "../TestUserMode/driverinterop.hpp"

struct Vector3 {
	float x, y, z;
};

int main() {
	HANDLE handle = {};
	std::cout << "finding driver";
	while ((handle = CreateFileW(L"\\\\.\\TestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "\nfound driver\n";
    DriverInterop driver(&handle, L"CPPRaylib.exe");
	Vector3 value{0.f, 0.f, 0.f};
	while (driver.is_valid()) {
		driver.read<Vector3>(157368980, value);
		printf_s("%f, %f, %f\n", value.x, value.y, value.z);
	}

	std::cin.get();

	return 0;
}