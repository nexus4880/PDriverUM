#pragma once
#include <Windows.h>
#include "../TestDriver/driver.h"
#include "utils.hpp"

class DriverInterop {
public:
	DriverInterop(PHANDLE pHandle, const wchar_t* processName) {
		this->m_pDriverHandle = pHandle;
		this->m_processId = GetProcessIdByName(processName);
	}

	~DriverInterop() {
		if (this->m_pDriverHandle) {
			CloseHandle(*this->m_pDriverHandle);
		}
	}

	bool is_valid() {
		return m_pDriverHandle != nullptr && *m_pDriverHandle != INVALID_HANDLE_VALUE && m_processId != 0;
	}

	template <typename T>
	bool read(long long address, const T& t) {
		if (!this->is_valid()) {
			return false;
		}

		_KERNEL_READ_REQUEST readRequest;
		readRequest.Address = address;
		readRequest.ProcessId = this->m_processId;
		readRequest.Size = sizeof(T);

		if (!DeviceIoControl(*this->m_pDriverHandle, IO_READ_REQUEST, &readRequest, sizeof(_KERNEL_READ_REQUEST), &readRequest, sizeof(_KERNEL_READ_REQUEST), 0, 0)) {
			return false;
		}

		memcpy((void*)&t, readRequest.Value, sizeof(T));

		return true;
	}

	template <typename T>
	bool write(long long address, const T& data) {
		if (!this->is_valid()) {
			return false;
		}

		_KERNEL_WRITE_REQUEST writeRequest;
		DWORD bytes;
		writeRequest.ProcessId = this->m_processId;
		writeRequest.Address = address;
		writeRequest.Value = (char*)malloc(sizeof(T));
		memcpy(writeRequest.Value, (void*)&data, sizeof(T));
		writeRequest.Size = sizeof(T);
		if (DeviceIoControl(*this->m_pDriverHandle, IO_WRITE_REQUEST, &writeRequest, sizeof(_KERNEL_WRITE_REQUEST), 0, 0, &bytes, NULL)) {
			return true;
		}

		return false;
	}

private:
	PHANDLE m_pDriverHandle;
	int m_processId;
};