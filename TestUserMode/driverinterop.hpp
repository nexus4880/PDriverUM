#pragma once
#include <Windows.h>
#include "../TestDriver/driver.h"
#include "utils.hpp"
#include <array>

typedef
#if AMD64
long long
#else
long
#endif
PTRW;

class DriverInterop {
public:
	DriverInterop(HANDLE handle, const wchar_t* processName) : handle(handle) {
		this->processId = GetProcessIdByName(processName);
	}

	~DriverInterop() {
		if (this->handle) {
			CloseHandle(this->handle);
		}
	}

	PTRW get_proc_base_address() {
		_KERNEL_GET_BASE_ADDRESS_REQUEST getBaseAddressRequest{this->processId, 0};
		DeviceIoControl(this->handle, IO_GET_BASE_ADDRESS_REQUEST, &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), 0, 0);

		return getBaseAddressRequest.BaseAddress;
	}

	template <typename T>
	bool read(PTRW address, const T& t) {
		_KERNEL_READ_REQUEST readRequest{this->processId, address, sizeof(T), (unsigned char*)&t};
		if (!DeviceIoControl(this->handle, IO_READ_REQUEST, &readRequest, sizeof(_KERNEL_READ_REQUEST), &readRequest, sizeof(_KERNEL_READ_REQUEST), 0, 0)) {
			return false;
		}

		return true;
	}

	template <typename T, size_t size>
	bool read_chain(PTRW address, std::array<PTRW, size> chain, const T& t) {
		if (size <= 0) {
			return false;
		}

		for (int i = 0; i < size; i++) {
			if (i != size - 1) {
				if (!this->read<PTRW>(address + chain[i], address)) {
					return false;
				}
			}
			else {
				return this->read<T>(address + chain[i], t);
			}
		}

		return false;
	}

	template <typename T>
	bool write(PTRW address, const T& data) {
		_KERNEL_WRITE_REQUEST writeRequest{this->processId, address, sizeof(T), (unsigned char*)&data};
		DWORD bytes = 0;

		return DeviceIoControl(this->handle, IO_WRITE_REQUEST, &writeRequest, sizeof(_KERNEL_WRITE_REQUEST), nullptr, NULL, &bytes, NULL);
	}

	PTRW GetModuleBaseAddress() {
		_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST request{0};
		DWORD bytes = 0;
		DeviceIoControl(this->handle, IO_GET_MODULE_BASE_REQUEST, &request, sizeof(_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST), &request, sizeof(_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST), &bytes, NULL);

		return request.Value;
	}
private:
	HANDLE handle;
	int processId;
};