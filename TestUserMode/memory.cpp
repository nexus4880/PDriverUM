#include "memory.h"

char* ReadMemoryA(HANDLE DriverHandle, unsigned long ProcId, long long Address, unsigned long size) {
	_KERNEL_READ_REQUEST kernelRead;
	kernelRead.Address = Address;
	kernelRead.ProcessId = ProcId;
	kernelRead.Size = size;

	if (DeviceIoControl(DriverHandle, IO_READ_REQUEST, &kernelRead, sizeof(kernelRead), &kernelRead, sizeof(kernelRead), 0, 0)) {
		return kernelRead.Value;
	}

	return nullptr;
}

template <typename T>
bool ReadMemory(HANDLE DriverHandle, unsigned long ProcId, long long Address, const T& t) {
	char* data = ReadMemoryA(DriverHandle, ProcId, Address, sizeof(T));
	if (!data) {
		return false;
	}

	memcpy((void*)&t, data, sizeof(T));
	return true;
}

bool WriteMemory(HANDLE driverHandle, unsigned long procId, long long address, PVOID pData, unsigned long size) {
	_KERNEL_WRITE_REQUEST KernelWrite;
	DWORD Bytes;
	KernelWrite.ProcessId = procId;
	KernelWrite.Address = address;
	KernelWrite.Value = (char*)malloc(size);
	memcpy(KernelWrite.Value, pData, size);
	KernelWrite.Size = size;
	if (DeviceIoControl(driverHandle, IO_WRITE_REQUEST, &KernelWrite, sizeof(KernelWrite), 0, 0, &Bytes, NULL)) {
		return true;
	}

	return false;
}


template <typename T>
bool WriteMemory(HANDLE DriverHandle, unsigned long ProcId, long long Address, const T& data) {
	return WriteMemory(DriverHandle, ProcId, Address, (PVOID)&data, sizeof(T));
}