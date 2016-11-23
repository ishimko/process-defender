#include <ntddk.h>
#include "Driver.h"
#include "Common.h"
#include "ProcessDefender.h"
#include "Functions.h"

PVOID RegistrationHandle = NULL;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("Driver entry.\n");

	UNICODE_STRING nameString, linkString;
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = STATUS_SUCCESS;

	DriverObject->DriverUnload = ProcessDefenderUnload;
	InitializeProcessDefender();
	
	RtlInitUnicodeString(&nameString, L"\\Device\\"DRIVER_NAME);
	status = IoCreateDevice(DriverObject, 0, &nameString, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
	if (NT_SUCCESS(status))
	{
		deviceObject->Flags |= DO_DIRECT_IO;

		RtlInitUnicodeString(&linkString, L"\\DosDevices\\"DRIVER_NAME);
		status = IoCreateSymbolicLink(&linkString, &nameString);

		if (NT_SUCCESS(status)) {
			DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatchIoctl;
			DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;

			InstallDefenderCallback();
		}
		else {
			DbgPrint("IoCreateSymbolicLink fail\n");
		}
	}
	else {
		DbgPrint("IoCreateDevice fail: ");
	}	

	return status;
}

VOID ProcessDefenderUnload(_In_ PDRIVER_OBJECT DriverObject) 
{
	UNREFERENCED_PARAMETER(DriverObject);

	UNICODE_STRING linkString;

	RemoveDefenderCallback();

	RtlInitUnicodeString(&linkString, L"\\DosDevices\\ProcDef");
	IoDeleteSymbolicLink(&linkString);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("Unloaded.\n");
}

NTSTATUS InstallDefenderCallback() 
{
	NTSTATUS result;	

	OB_OPERATION_REGISTRATION OperationRegistration;
	memset(&OperationRegistration, 0, sizeof(OperationRegistration));
	OperationRegistration.ObjectType = PsProcessType;
	OperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE;
	OperationRegistration.PreOperation = ObjectPreCallback;	
	
	OB_CALLBACK_REGISTRATION CallbackRegistration;
	memset(&CallbackRegistration, 0, sizeof(CallbackRegistration));
	CallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
	CallbackRegistration.OperationRegistrationCount = 1;
	UNICODE_STRING Altitude;
	RtlInitUnicodeString(&Altitude, L"XXXXXXX");
	CallbackRegistration.Altitude = Altitude;
	CallbackRegistration.OperationRegistration = &OperationRegistration;	

	result = ObRegisterCallbacks(&CallbackRegistration, &RegistrationHandle);

	if (!NT_SUCCESS(result)) 
	{
		DbgPrint("ObRegisterCallbacks fail\n");
	}

	return result;
}

NTSTATUS RemoveDefenderCallback()
{
	if (RegistrationHandle != NULL) 
	{
		ObUnRegisterCallbacks(RegistrationHandle);
		RegistrationHandle = NULL;
	}
	return STATUS_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	STRING ProcessName;

	LPCSTR ProcessNameStr = PsGetProcessImageFileName((PEPROCESS)OperationInformation->Object);
	RtlInitString(&ProcessName, ProcessNameStr);

	POB_PRE_CREATE_HANDLE_INFORMATION CreateHandleInformation = &(OperationInformation->Parameters->CreateHandleInformation);
	if ((OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) && IsProtectedProcess(&ProcessName))
	{		
		CreateHandleInformation->DesiredAccess = FilterAccess(CreateHandleInformation->OriginalDesiredAccess);
		if (CreateHandleInformation->DesiredAccess != CreateHandleInformation->OriginalDesiredAccess) {
			DbgPrint("Protection hit\n");
		}
	}
	
	return OB_PREOP_SUCCESS;
}