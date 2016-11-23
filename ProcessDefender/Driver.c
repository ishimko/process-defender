#include <ntddk.h>
#include "Driver.h"
#include "Common.h"
#include "ProcessDefender.h"
#include "DriverFunctions.h"

PDRIVER_OBJECT gDriverObject = NULL;

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING nameString, linkString;
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = STATUS_SUCCESS;
	PVOID driverObjectExtension;
	gDriverObject = DriverObject;

	gDriverObject->DriverUnload = ProcessDefenderUnload;
	
	RtlInitUnicodeString(&nameString, L"\\Device\\"DRIVER_NAME);
	status = IoCreateDevice(gDriverObject, 0, &nameString, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
	ASSERT(NT_SUCCESS(status));

	deviceObject->Flags |= DO_DIRECT_IO;

	RtlInitUnicodeString(&linkString, L"\\DosDevices\\"DRIVER_NAME);
	status = IoCreateSymbolicLink(&linkString, &nameString);
	ASSERT(NT_SUCCESS(status));

	gDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcessDefenderDispatchIoctl;
	gDriverObject->MajorFunction[IRP_MJ_CREATE] = ProcessDefenderDispatchCreate;
	gDriverObject->MajorFunction[IRP_MJ_CLOSE] = ProcessDefenderDispatchClose;

	status = IoAllocateDriverObjectExtension(DriverObject, (PVOID)PROCESSDEFENDER_DRIVER_OBJECT_EXTENSION_ID, sizeof(DRIVER_DATA), &driverObjectExtension);
	ASSERT(NT_SUCCESS(status));

	InitializeDriverData((PDRIVER_DATA)driverObjectExtension);
	InstallDefenderCallback();
		
	return status;
}

VOID ProcessDefenderUnload(_In_ PDRIVER_OBJECT DriverObject) 
{
	UNICODE_STRING linkString;

	RemoveDefenderCallback();

	RtlInitUnicodeString(&linkString, L"\\DosDevices\\"DRIVER_NAME);
	IoDeleteSymbolicLink(&linkString);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint(DBG_PREFIX"unloaded\n");
}

VOID InitializeDriverData(_Inout_ PDRIVER_DATA DriverData)
{
	RtlZeroMemory(DriverData, sizeof(DRIVER_DATA));
}

PDRIVER_DATA GetDriverData()
{
	return (PDRIVER_DATA)IoGetDriverObjectExtension(gDriverObject, (PVOID)PROCESSDEFENDER_DRIVER_OBJECT_EXTENSION_ID);
}

NTSTATUS InstallDefenderCallback() 
{
	NTSTATUS result;	
	PDRIVER_DATA driverData = GetDriverData();

	OB_OPERATION_REGISTRATION OperationRegistration;
	RtlZeroMemory(&OperationRegistration, sizeof(OperationRegistration));
	OperationRegistration.ObjectType = PsProcessType;
	OperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE;
	OperationRegistration.PreOperation = ProcessDefenderObjectPreCallback;	
	
	OB_CALLBACK_REGISTRATION CallbackRegistration;
	RtlZeroMemory(&CallbackRegistration, sizeof(CallbackRegistration));
	CallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
	CallbackRegistration.OperationRegistrationCount = 1;
	UNICODE_STRING Altitude;
	RtlInitUnicodeString(&Altitude, L"XXXXXXX");
	CallbackRegistration.Altitude = Altitude;
	CallbackRegistration.OperationRegistration = &OperationRegistration;	
	
	result = ObRegisterCallbacks(&CallbackRegistration, &(driverData->RegistrationHandle));

	if (!NT_SUCCESS(result)) 
	{
		DbgPrint(DBG_PREFIX"callback registration failed\n");
	}
	else {
		DbgPrint(DBG_PREFIX"callback registered\n");
	}

	return result;
}

NTSTATUS RemoveDefenderCallback()
{
	PDRIVER_DATA driverData = GetDriverData();

	if (driverData != NULL) 
	{
		ObUnRegisterCallbacks(driverData->RegistrationHandle);
		driverData->RegistrationHandle = NULL;
	}

	return STATUS_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS ProcessDefenderObjectPreCallback(_In_ PVOID RegistrationContext, _In_ POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	STRING processName;
	PPROCESS_DEFENDER_OBJECT processDefenderObject = &(GetDriverData()->ProcessDefenderObject);

	LPCSTR processNameStr = PsGetProcessImageFileName((PEPROCESS)OperationInformation->Object);
	RtlInitString(&processName, processNameStr);

	POB_PRE_CREATE_HANDLE_INFORMATION createHandleInformation = &(OperationInformation->Parameters->CreateHandleInformation);
	if ((OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) && IsProtectedProcess(processDefenderObject, &processName))
	{		
		createHandleInformation->DesiredAccess = FilterAccess(createHandleInformation->OriginalDesiredAccess);
		if (createHandleInformation->DesiredAccess != createHandleInformation->OriginalDesiredAccess) {
			DbgPrint(DBG_PREFIX"protection hit\n");
		}
	}
	
	return OB_PREOP_SUCCESS;
}