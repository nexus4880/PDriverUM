#include "../TestDriver/driver.h"
#include <iostream>
#include <Windows.h>
#include <thread>
#include "driverinterop.hpp"

PHANDLE pHandle;

int print_error(const char* message) {
	if (pHandle) {
		CloseHandle(*pHandle);
	}

	std::cout << message << std::endl;
	std::cin.get();

	return -1;
}

struct Vector3 {
	float x, y, z;
};

int main()
{
	std::cout << "finding driver";
	HANDLE handle = {};
	while ((handle = CreateFile(L"\\\\.\\TestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	pHandle = &handle;
	std::cout << "\nfound driver\n";
	DriverInterop interop(&handle, L"TestApplication.exe");
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		int number = 0;
		if (!interop.read<int>(0x00007FF787385630, number)) {
			throw;
		}

		printf_s("read %i\n", number);
	}

	std::cin.get();

	return 0;
}