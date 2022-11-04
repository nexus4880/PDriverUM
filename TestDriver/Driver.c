#pragma warning (disable : 4100 4101 4242 4244)
#include "Driver.h"
#include "Memory.h"

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DbgPrintEx(0, 0, "Initializing TestDriver");
	RtlInitUnicodeString(&dev, L"\\Device\\TestDriver");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\TestDriver");
	NTSTATUS status;
	if (!NT_SUCCESS(status = IoCreateDevice(DriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject))) {
		DbgPrintEx(0, 0, "IoCreateDevice failed");

		return status;
	}

	DbgPrintEx(0, 0, "IoCreateDevice success");
	if (!NT_SUCCESS(status = IoCreateSymbolicLink(&dos, &dev))) {
		IoDeleteDevice(pDeviceObject);
		DbgPrintEx(0, 0, "IoCreateSymbolicLink failed");

		return status;
	}

	DbgPrintEx(0, 0, "IoCreateSymbolicLink success");
	if (!NT_SUCCESS(status = PsSetLoadImageNotifyRoutine(ImageLoadCallback))) {
		IoDeleteDevice(pDeviceObject);
		IoDeleteSymbolicLink(&dos);
		DbgPrintEx(0, 0, "PsSetLoadImageNotifyRoutine failed");

		return status;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	DriverObject->DriverUnload = UnloadDriver;
	DriverObject->Flags |= DO_DIRECT_IO;
	DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DbgPrintEx(0, 0, "Initialized TestDriver");

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading TestDriver");
	if (PsRemoveLoadImageNotifyRoutine(ImageLoadCallback) != STATUS_SUCCESS) {
		DbgPrintEx(0, 0, "Failed to remove load image routine");
	}
	if (IoDeleteSymbolicLink(&dos) != STATUS_SUCCESS) {
		DbgPrintEx(0, 0, "Failed to delete symbolic link");
	}
	
	if (pDriverObject) {
		IoDeleteDevice(pDriverObject->DeviceObject);
	}

	DbgPrintEx(0, 0, "Unloaded TestDriver");

	return STATUS_SUCCESS;
}

void ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	/*
	if (wcsstr(FullImageName->Buffer, L"\\csgo\\bin\\client.dll")) {
		DbgPrintEx(0, 0, "Process started %ls", FullImageName->Buffer);
	}
	*/
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DbgPrintEx(0, 0, "IoControl");
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (controlCode == IO_RANDOM_NUMBER_REQUEST) {
		DbgPrintEx(0, 0, "IOCTL_SIOCTL_METHOD_BUFFERED");
		PRANDOM_NUMBER_REQUEST data = (PRANDOM_NUMBER_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		data->Value = 69420;
		bytes = sizeof(RANDOM_NUMBER_REQUEST);
	} else if (controlCode == IO_READ_REQUEST) {
		DbgPrintEx(0, 0, "IO_READ_REQUEST");
		PKERNEL_READ_REQUEST data = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PEPROCESS proc;
		status = PsLookupProcessByProcessId((HANDLE)data->ProcessId, &proc);
		if (NT_SUCCESS(status)) {
			status = KeReadProcessMemory(proc, (PVOID)data->Address, &data->Response, data->Size);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(0, 0, "failed to KeReadProcessMemory");
			}
		}
		else {
			DbgPrintEx(0, 0, "failed to PsLookupProcessByProcessId");
		}

		DbgPrintEx(0, 0, "Read Params:  %lu, %lld \n", data->ProcessId, data->Address);
		DbgPrintEx(0, 0, "Value: %lu \n", data->Response);

		bytes = sizeof(KERNEL_READ_REQUEST);
	} else if (controlCode == IO_WRITE_REQUEST) {
		DbgPrintEx(0, 0, "IO_WRITE_REQUEST");
		PKERNEL_WRITE_REQUEST WriteInput = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)WriteInput->ProcessId, &Process)))
			KeWriteProcessMemory(Process, &WriteInput->Value,
				(PVOID)WriteInput->Address, WriteInput->Size);

		DbgPrintEx(0, 0, "Write Params:  %lu, %#010x \n", WriteInput->Value, WriteInput->Address);

		bytes = sizeof(KERNEL_WRITE_REQUEST);
	}
	else {
		DbgPrintEx(0, 0, "Unhandled code %ul", controlCode);
		status = STATUS_INVALID_PARAMETER;
		bytes = 0;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = bytes;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	DbgPrintEx(0, 0, "CreateCall");
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	DbgPrintEx(0, 0, "CloseCall");
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}