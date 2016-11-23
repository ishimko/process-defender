#pragma once

#include <ntddk.h>

extern LPCSTR PsGetProcessImageFileName(_In_ PEPROCESS Process);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID ProcessDefenderUnload(PDRIVER_OBJECT DriverObject);

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation);

NTSTATUS InstallDefenderCallback();
NTSTATUS RemoveDefenderCallback();
