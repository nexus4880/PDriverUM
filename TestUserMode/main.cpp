#include "../TestDriver/driver.h"
#include <iostream>
#include <Windows.h>
#include <thread>
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
	if (WriteMemory<Vector3>(handle, 9144, 0x00007FF787385040, Vector3{ 2.3f, 4.6f, 8.10f })) {
		std::cout << "wrote" << std::endl;
	}
	else {
		std::cout << "failed to write" << std::endl;
	}



	std::cin.get();











	return 0;
	int error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND) {
		return print_error("failed to open driver");
	}

	std::cout << "read (0) or write (1) or get base address (2): ";
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
				int value = 0;
				std::cout << "read " << ReadMemory<int>(handle, pid, address, value) << "\n";
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
				if (!WriteMemory<int>(handle, pid, address, value)) {
					return print_error("failed to write");
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}

			break;
		}
		case 2: {
			_KERNEL_GET_BASE_ADDRESS_REQUEST getBaseAddressRequest;
			getBaseAddressRequest.ProcessId = pid;
			getBaseAddressRequest.BaseAddress = 0;
			if (DeviceIoControl(handle, IO_GET_BASE_ADDRESS_REQUEST, &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), 0, 0, NULL, NULL)) {
				std::cout << "read " << getBaseAddressRequest.BaseAddress << std::endl;
			}
			else {
				return print_error("failed to make request");
			}
			std::cin.get();
			break;
		}
		default: {
			return print_error("invalid input");
		}
	}
	
	CloseHandle(handle);

	return 0;
}