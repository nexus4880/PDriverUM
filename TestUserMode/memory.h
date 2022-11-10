#pragma once
#include <Windows.h>
#include "../TestDriver/driver.h"

char* ReadMemoryA(HANDLE DriverHandle, unsigned long ProcId, long long Address, unsigned long size);

template <typename T>
bool ReadMemory(HANDLE DriverHandle, unsigned long ProcId, long long Address, const T& t);

bool WriteMemory(HANDLE driverHandle, unsigned long procId, long long address, PVOID pData, unsigned long size);

template <typename T>
bool WriteMemory(HANDLE DriverHandle, unsigned long ProcId, long long Address, const T& data);