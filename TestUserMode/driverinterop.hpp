#pragma once
#include <Windows.h>
#include "../TestDriver/driver.h"
#include "utils.hpp"
#include <array>

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

	driver_ptr_t GetProcessBaseAddress() {
		_KERNEL_GET_BASE_ADDRESS_REQUEST getBaseAddressRequest{this->processId, 0};
		DeviceIoControl(this->handle, IO_GET_BASE_ADDRESS_REQUEST, &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), 0, 0);

		return getBaseAddressRequest.BaseAddress;
	}

	template <typename T>
	bool Read(driver_ptr_t address, T* tPtr) {
		if (!address || !tPtr) {
			return false;
		}

		_KERNEL_READ_REQUEST request{this->processId, address, sizeof(T), (unsigned char*)tPtr};
		
		return DeviceIoControl(this->handle, IO_READ_REQUEST, &request, sizeof(_KERNEL_READ_REQUEST), &request, sizeof(_KERNEL_READ_REQUEST), 0, 0);
	}

	template <typename T, size_t size>
	bool ReadChain(driver_ptr_t address, std::array<driver_ptr_t, size> chain, T* tPtr) {
		if (size <= 1) {
			return false;
		}

		for (int i = 0; i < size; i++) {
			if (i != size - 1) {
				if (!this->Read<driver_ptr_t>(address + chain[i], &address)) {
					return false;
				}
			}
			else {
				return this->Read<T>(address + chain[i], tPtr);
			}
		}

		return false;
	}

	template <typename T, size_t size>
	driver_ptr_t ReadChainAddress(driver_ptr_t address, std::array<driver_ptr_t, size> chain) {
		if (!address || size <= 0) {
			return 0;
		}

		for (int i = 0; i < size; i++) {
			if (i != size - 1) {
				if (!this->Read<driver_ptr_t>(address + chain[i], &address)) {
					return 0;
				}
			}
			else {
				return address + chain[i];
			}
		}

		return 0;
	}

	template <typename T>
	bool Write(driver_ptr_t address, const T& value) {
		if (!address) {
			return false;
		}

		_KERNEL_WRITE_REQUEST request{this->processId, address, sizeof(T), (unsigned char*)&value};
		
		return DeviceIoControl(this->handle, IO_WRITE_REQUEST, &request, sizeof(_KERNEL_WRITE_REQUEST), nullptr, NULL, nullptr, NULL);
	}

	driver_ptr_t GetModuleBaseAddress() {
		driver_ptr_t request = 0;
		DeviceIoControl(this->handle, IO_GET_MODULE_BASE_REQUEST, &request, sizeof(driver_ptr_t), &request, sizeof(driver_ptr_t), nullptr, NULL);

		return request;
	}
private:
	HANDLE handle;
	DWORD processId;
};