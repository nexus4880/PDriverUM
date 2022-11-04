#include "Driver.h"
#include <iostream>
#include "memory.h"

PHANDLE pHandle;

int print_error(const char* message) {
	if (pHandle) {
		CloseHandle(*pHandle);
	}

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

	pHandle = &handle;
	std::cout << "\nfound driver\n";
	int error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND) {
		return print_error("failed to open driver");
	}

	std::cout << "read (0) or write (1): ";
	int method = 0;
	std::cin >> method;
	ULONG pid = 0;
	std::cout << "pid: ";
	std::cin >> pid;
	long long address = 0;
	std::cout << "address: ";
	std::cin >> address;
	ULONG size = 0;
	std::cout << "what is the size of the value: ";
	std::cin >> size;
	switch (method) {
		case 0: {
		std::cout << "running, press escape to quit\n";
			while (!(GetAsyncKeyState(VK_ESCAPE) & 1)) {
				std::cout << "read " << ReadMemory(handle, pid, address, size) << "\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			break;
		}
		case 1: {
			int value = 0;
			std::cout << "what value do you want to write: ";
			std::cin >> value;
			std::cout << "running, press escape to quit\n";
			while (!(GetAsyncKeyState(VK_ESCAPE) & 1)) {
				if (!WriteMemory(handle, pid, address, value, size)) {
					return print_error("failed to write");
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			break;
		}
		default: {
			return print_error("invalid input");
		}
	}
	
	CloseHandle(handle);

	return 0;
}