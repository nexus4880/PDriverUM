#pragma once
#include "stdafx.h"

typedef struct _RANDOM_NUMBER_REQUEST {
	int Value;
} RANDOM_NUMBER_REQUEST, * PRANDOM_NUMBER_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	ULONG ProcessId;
	long long Address;
	ULONG Response;
	ULONG Size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONG ProcessId;
	long long Address;
	ULONG Value;
	ULONG Size;

} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

#define SIOCTL_TYPE 40000
#define IO_RANDOM_NUMBER_REQUEST CTL_CODE( SIOCTL_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_READ_REQUEST CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(SIOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)