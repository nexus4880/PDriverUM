#pragma once
#include "stdafx.h"
#include "Driver.h"

ULONG ReadMemory(HANDLE DriverHandle, ULONG ProcId, long long Address, ULONG size) {
	_KERNEL_READ_REQUEST kernelRead;
	kernelRead.Address = Address;
	kernelRead.ProcessId = ProcId;
	kernelRead.Size = size;

	if (DeviceIoControl(DriverHandle, IO_READ_REQUEST, &kernelRead, sizeof(kernelRead), &kernelRead, sizeof(kernelRead), 0, 0)) {
		return kernelRead.Value;
	}

	return 0;
}

template <typename T>
T ReadMemory(HANDLE DriverHandle, ULONG ProcId, long long Address) {
	return (T)ReadMemory(DriverHandle, ProcId, Address, sizeof(T));
}

bool WriteMemory(HANDLE DriverHandle, ULONG ProcId, long long Address, ULONG Value, ULONG Size) {
	KERNEL_WRITE_REQUEST KernelWrite;
	DWORD Bytes;
	KernelWrite.ProcessId = ProcId;
	KernelWrite.Address = Address;
	KernelWrite.Value = Value;
	KernelWrite.Size = Size;
	if (DeviceIoControl(DriverHandle, IO_WRITE_REQUEST, &KernelWrite, sizeof(KernelWrite), 0, 0, &Bytes, NULL)) {
		return true;
	}

	return false;
}


template <typename T>
bool WriteMemory(HANDLE DriverHandle, ULONG ProcId, long long Address, T t) {
	return WriteMemory(DriverHandle, ProcId, Address, (ULONG)t, sizeof(T));
}