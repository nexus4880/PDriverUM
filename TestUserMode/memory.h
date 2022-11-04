#pragma once
#include "stdafx.h"
#include "Driver.h"

template <typename T>
T ReadMemory(HANDLE DriverHandle, ULONG ProcId, long long Address) {
	_KERNEL_READ_REQUEST kernelRead;
	kernelRead.Address = Address;
	kernelRead.ProcessId = ProcId;
	kernelRead.Size = sizeof(T);

	if (DeviceIoControl(DriverHandle, IO_READ_REQUEST, &kernelRead, sizeof(kernelRead), &kernelRead, sizeof(kernelRead), 0, 0)) {
		return (T)kernelRead.Value;
	}

	return (T)false;
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