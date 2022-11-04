#include "Driver.h"
#include <iostream>
#include "memory.h"

int print_error(const char* message) {
	std::cout << message << std::endl;
	std::cin.get();

	return -1;
}

int main()
{
	std::cout << "finding driver";
	HANDLE handle = {};
	while ((handle = CreateFile(L"\\\\.\\TestDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		std::cout << ".";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "\nfound driver\n";
	int error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND) {
		return print_error("failed to open driver");
	}

	
	while (true) {

		std::cout << "read " << ReadMemory<bool>(handle, (ULONG)16896, 0x00007FF7C2365034) << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	
	CloseHandle(handle);
	return 0;
}