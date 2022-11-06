#pragma once

#ifdef DRIVER
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);
void ImageLoadCallback(PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO pImageInfo);
NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
#endif

#define SIOCTL_TYPE 40000
#define IO_READ_REQUEST CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_GET_BASE_ADDRESS_REQUEST CTL_CODE(SIOCTL_TYPE, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)

#ifdef DRIVER
typedef 
#endif
struct _KERNEL_READ_REQUEST
{
	unsigned long ProcessId;
	long long Address;
	char Value[32];
	unsigned long Size;
}

#ifdef DRIVER
KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST
#endif
;

#ifdef DRIVER
typedef
#endif
struct _KERNEL_WRITE_REQUEST
{
	unsigned long ProcessId;
	long long Address;
	char* Value;
	unsigned long Size;
}

#ifdef DRIVER
KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST
#endif
;

#ifdef DRIVER
typedef
#endif
struct _KERNEL_GET_BASE_ADDRESS_REQUEST
{
	unsigned long ProcessId;
	long long BaseAddress;
}

#ifdef DRIVER
KERNEL_GET_BASE_ADDRESS_REQUEST, * PKERNEL_GET_BASE_ADDRESS_REQUEST
#endif
;