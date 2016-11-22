#pragma once

#include <ntddk.h>

PVOID RegistrationHandle = NULL;

extern LPCSTR PsGetProcessImageFileName(_In_ PEPROCESS Process);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID ProcessDefenderUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS EnableDefence();
NTSTATUS DisableDefence();
OB_PREOP_CALLBACK_STATUS ObjectPreCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation);