#include <ntddk.h>
#include "Driver.h"

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("Driver entry\n");
	
	DriverObject->DriverUnload = ProcessDefenderUnload;

	return STATUS_SUCCESS;
}

VOID ProcessDefenderUnload(_In_ PDRIVER_OBJECT DriverObject) 
{
	UNREFERENCED_PARAMETER(DriverObject);

	DbgPrint("Driver unload\n");
}