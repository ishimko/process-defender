#pragma once

#include <ntddk.h>

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID ProcessDefenderUnload(PDRIVER_OBJECT DriverObject);
