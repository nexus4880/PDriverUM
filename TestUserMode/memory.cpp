#include "memory.h"
template <typename T>
T ReadMemory(HANDLE DriverHandle, ULONG ProcId, long long Address, ULONG Size) {
	_KERNEL_READ_REQUEST kernelRead;
	kernelRead.Address = Address;
	kernelRead.ProcessId = ProcId;
	kernelRead.Size = Size;

	if (DeviceIoControl(DriverHandle, IO_READ_REQUEST, &kernelRead, sizeof(kernelRead), &kernelRead, sizeof(kernelRead), 0, 0)) {
		return (T)kernelRead.Response;
	}

	return (T)false;
}