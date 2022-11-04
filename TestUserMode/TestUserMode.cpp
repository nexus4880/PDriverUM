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

	std::cout << "\n";
	int error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND) {
		return print_error("failed to open driver");
	}

	/*while (true) {
		DWORD bytesReturned = 0;
		RANDOM_NUMBER_REQUEST request = {};
		if (!DeviceIoControl(handle, IO_RANDOM_NUMBER_REQUEST, &request, sizeof(KERNEL_READ_REQUEST), &request, sizeof(KERNEL_READ_REQUEST), &bytesReturned, NULL)) {
			return print_error("failed to open ioctl");
		}

		std::cout << request.Value << " was our request" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}*/

	ULONG procid = 19276;
	WriteMemory(handle, procid, 0x000000B94E6FF780, 20, sizeof(int));
	int k = ReadMemory<int>(handle, procid, 0x000000B94E6FF780, sizeof(int));
	std::cout << k;
	
	CloseHandle(handle);
	getchar();
	return 0;
}
