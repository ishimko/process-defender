#pragma once

#include <ntddk.h>
#include "ProcessDefender.h"

#define PROCESSDEFENDER_DRIVER_OBJECT_EXTENSION_ID 1
#define DBG_PREFIX "ProcessDefender: "

extern LPCSTR PsGetProcessImageFileName(_In_ PEPROCESS Process);

typedef struct _DRIVER_DATA{
	PROCESS_DEFENDER_OBJECT ProcessDefenderObject;
	PVOID RegistrationHandle;
} DRIVER_DATA, *PDRIVER_DATA;

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID ProcessDefenderUnload(PDRIVER_OBJECT DriverObject);

VOID InitializeDriverData(PDRIVER_DATA DriverData);

PDRIVER_DATA GetDriverData();

OB_PREOP_CALLBACK_STATUS ProcessDefenderObjectPreCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation);

NTSTATUS InstallDefenderCallback();
NTSTATUS RemoveDefenderCallback();
