#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

#define SIOCTL_TYPE 40000
#define IO_READ_REQUEST CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _KERNEL_READ_REQUEST
{
	ULONG ProcessId;
	long long Address;
	ULONG Value;
	ULONG Size;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONG ProcessId;
	long long Address;
	ULONG Value;
	ULONG Size;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);
void ImageLoadCallback(PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO pImageInfo);
NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
