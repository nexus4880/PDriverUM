#pragma warning (disable : 4100 4047)

#define POOL_TAG 1582
#define PRINT_DEBUG 0
#define PRINT_ERRORS 1
#define DRIVER 1
#include "driver.h"
#include "memory.h"

PDEVICE_OBJECT _pDeviceObject;
UNICODE_STRING _dev, _dos;
long long baseModuleAddress = 0;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING registryPath) {
	DbgPrintEx(0, 0, "Initializing TestDriver");
	RtlInitUnicodeString(&_dev, L"\\Device\\TestDriver");
	RtlInitUnicodeString(&_dos, L"\\DosDevices\\TestDriver");
	NTSTATUS status = IoCreateDevice(pDriverObject, 0, &_dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &_pDeviceObject);
	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, "IoCreateDevice failed");

		return status;
	}

	DbgPrintEx(0, 0, "IoCreateDevice success");
	status = IoCreateSymbolicLink(&_dos, &_dev);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(_pDeviceObject);
		DbgPrintEx(0, 0, "IoCreateSymbolicLink failed");

		return status;
	}

	DbgPrintEx(0, 0, "IoCreateSymbolicLink success");
	status = PsSetLoadImageNotifyRoutine(ImageLoadCallback);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(_pDeviceObject);
		IoDeleteSymbolicLink(&_dos);
		DbgPrintEx(0, 0, "PsSetLoadImageNotifyRoutine failed");

		return status;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;
	pDriverObject->Flags |= DO_DIRECT_IO;
	pDriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DbgPrintEx(0, 0, "Initialized TestDriver");

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {
	DbgPrintEx(0, 0, "Unloading TestDriver");
	if (!NT_SUCCESS(PsRemoveLoadImageNotifyRoutine(ImageLoadCallback))) {
		DbgPrintEx(0, 0, "Failed to remove load image routine");
	}

	if (!NT_SUCCESS(IoDeleteSymbolicLink(&_dos))) {
		DbgPrintEx(0, 0, "Failed to delete symbolic link");
	}
	
	if (pDriverObject) {
		IoDeleteDevice(pDriverObject->DeviceObject);
	}

	DbgPrintEx(0, 0, "Unloaded TestDriver");

	return STATUS_SUCCESS;
}

// (wcsstr(FullImageName->Buffer, L"\\path\\to\\file.dll")
void ImageLoadCallback(PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO pImageInfo) {
	if (wcsstr(fullImageName->Buffer, L"UnityPlayer.dll")) {
		baseModuleAddress = (long long)pImageInfo->ImageBase;
		DbgPrintEx(0, 0, "Found base module address: %lli", baseModuleAddress);
	}
}

NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp) {
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	PEPROCESS proc;
	switch (controlCode) {
		case IO_READ_REQUEST: {
			PKERNEL_READ_REQUEST readRequest = (PKERNEL_READ_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
			status = PsLookupProcessByProcessId((HANDLE)readRequest->ProcessId, &proc);
			if (NT_SUCCESS(status)) {
				DbgPrintEx(0, 0, "Attempting to read %i bytes", readRequest->Size);
				status = KeReadProcessMemory(proc, (PVOID)readRequest->Address, readRequest->Value, readRequest->Size);
				if (!NT_SUCCESS(status)) {
#if PRINT_ERRORS
					DbgPrintEx(0, 0, "failed to KeReadProcessMemory");
#endif
				}
			}
			else {
#if PRINT_ERRORS
				DbgPrintEx(0, 0, "failed to PsLookupProcessByProcessId");
#endif
			}

#if PRINT_DEBUG
			DbgPrintEx(0, 0, "IO_READ_REQUEST: PID(%lu) | ADDRESS(%lli) | SIZE(%i)", readRequest->ProcessId, readRequest->Address, readRequest->Size);
#endif
			bytes = sizeof(KERNEL_READ_REQUEST);
			break;
		}
		case IO_WRITE_REQUEST: {
			PKERNEL_WRITE_REQUEST writeRequest = (PKERNEL_WRITE_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
			PEPROCESS Process;
			status = PsLookupProcessByProcessId((HANDLE)writeRequest->ProcessId, &Process);
			if (NT_SUCCESS(status)) {
				status = KeWriteProcessMemory(Process, writeRequest->Value, (PVOID)writeRequest->Address, writeRequest->Size);
				if (!NT_SUCCESS(status)) {
#if PRINT_ERRORS
					DbgPrintEx(0, 0, "KeWriteProcessMemory failed");
#endif
				}
			}
			else {
#if PRINT_ERRORS
				DbgPrintEx(0, 0, "PsLookupProcessByProcessId failed");
#endif
			}

#if PRINT_DEBUG
			DbgPrintEx(0, 0, "IO_WRITE_REQUEST: PID(%lu) | ADDRESS(%lli) | SIZE(%i)", writeRequest->ProcessId, writeRequest->Address, writeRequest->Size);
#endif
			bytes = sizeof(KERNEL_WRITE_REQUEST);
			break;
		}
		case IO_GET_BASE_ADDRESS_REQUEST: {
			PKERNEL_GET_BASE_ADDRESS_REQUEST getBaseAddressRequest = (PKERNEL_GET_BASE_ADDRESS_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
			PEPROCESS Process;
			status = PsLookupProcessByProcessId((HANDLE)getBaseAddressRequest->ProcessId, &Process);
			getBaseAddressRequest->BaseAddress = 0;
			if (NT_SUCCESS(status)) {
				getBaseAddressRequest->BaseAddress = PsGetProcessSectionBaseAddress(Process);
			}
			else {
#if PRINT_ERRORS
				DbgPrintEx(0, 0, "PsLookupProcessByProcessId failed");
#endif
			}

#if PRINT_DEBUG
			DbgPrintEx(0, 0, "IO_GET_BASE_ADDRESS_REQUEST: PID(%lu) | BASE_ADDRESS(%lli)", getBaseAddressRequest->ProcessId, getBaseAddressRequest->BaseAddress);
#endif
			bytes = sizeof(KERNEL_GET_BASE_ADDRESS_REQUEST);
			break;
		}
		case IO_GET_MODULE_BASE_REQUEST: {
			PKERNEL_GET_BASE_MODULE_ADDRESS_REQUEST getBaseAddressRequest = (PKERNEL_GET_BASE_MODULE_ADDRESS_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
			if (baseModuleAddress) {
				getBaseAddressRequest->Value = baseModuleAddress;
				bytes = sizeof(KERNEL_GET_BASE_MODULE_ADDRESS_REQUEST);
			}
			else {
				status = STATUS_CONNECTION_DISCONNECTED;
			}

			break;
		}
		default: {
			DbgPrintEx(0, 0, "Unhandled code %ul", controlCode);
			status = STATUS_INVALID_PARAMETER;
			bytes = 0;
			break;
		}
	}
	
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = bytes;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp) {
	DbgPrintEx(0, 0, "CreateCall");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP pIrp) {
	DbgPrintEx(0, 0, "CloseCall");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}