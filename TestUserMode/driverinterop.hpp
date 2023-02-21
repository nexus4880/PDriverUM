#pragma once
#include <Windows.h>
#include "../TestDriver/driver.h"
#include "utils.hpp"

typedef
#if AMD64
long long
#else
long
#endif
PTRW;

class DriverInterop {
public:
	DriverInterop(PHANDLE pHandle, const wchar_t* processName) : m_pDriverHandle(pHandle) {
		this->m_processId = GetProcessIdByName(processName);
	}

	~DriverInterop() {
		if (this->m_pDriverHandle) {
			CloseHandle(*this->m_pDriverHandle);
		}
	}

	PTRW get_proc_base_address() {
		_KERNEL_GET_BASE_ADDRESS_REQUEST getBaseAddressRequest{this->m_processId, 0};
		DeviceIoControl(*this->m_pDriverHandle, IO_GET_BASE_ADDRESS_REQUEST, &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), &getBaseAddressRequest, sizeof(_KERNEL_GET_BASE_ADDRESS_REQUEST), 0, 0);

		return getBaseAddressRequest.BaseAddress;
	}

	bool read_with_size(PTRW address, void* t, const int& size) {
		_KERNEL_READ_REQUEST readRequest{this->m_processId, address, size};
		if (!DeviceIoControl(*this->m_pDriverHandle, IO_READ_REQUEST, &readRequest, sizeof(_KERNEL_READ_REQUEST), &readRequest, sizeof(_KERNEL_READ_REQUEST), 0, 0)) {
			return false;
		}

		memcpy(t, readRequest.Value, size);

		return true;
	}

	template <typename T>
	bool read(PTRW address, const T& t) {
		_KERNEL_READ_REQUEST readRequest{this->m_processId, address, sizeof(T)};
		if (!DeviceIoControl(*this->m_pDriverHandle, IO_READ_REQUEST, &readRequest, sizeof(_KERNEL_READ_REQUEST), &readRequest, sizeof(_KERNEL_READ_REQUEST), 0, 0)) {
			return false;
		}

		memcpy((void*)&t, readRequest.Value, sizeof(T));

		return true;
	}

	template <typename T>
	bool read_chain(PTRW address, PTRW* chain, size_t chainSize, const T& t) {
		if (chainSize <= 0) {
			return false;
		}

		for (int i = 0; i < chainSize; i++) {
			if (i != chainSize - 1) {
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
		_KERNEL_WRITE_REQUEST writeRequest{this->m_processId, address, sizeof(T), (char*)&data};
		DWORD bytes = 0;

		return DeviceIoControl(*this->m_pDriverHandle, IO_WRITE_REQUEST, &writeRequest, sizeof(_KERNEL_WRITE_REQUEST), nullptr, NULL, &bytes, NULL);
	}

	PTRW GetModuleBaseAddress() {
		_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST request{0};
		DWORD bytes = 0;
		DeviceIoControl(*this->m_pDriverHandle, IO_GET_MODULE_BASE_REQUEST, &request, sizeof(_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST), &request, sizeof(_KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST), &bytes, NULL);

		return request.Value;
	}
private:
	PHANDLE m_pDriverHandle;
	int m_processId;
};