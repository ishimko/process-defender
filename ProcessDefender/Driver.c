#include <ntddk.h>
#include "Driver.h"
#include "ProcessDefender.h"

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("Driver entry.\n");
	
	DriverObject->DriverUnload = ProcessDefenderUnload;

	EnableDefence();

	return STATUS_SUCCESS;
}

VOID ProcessDefenderUnload(_In_ PDRIVER_OBJECT DriverObject) 
{
	UNREFERENCED_PARAMETER(DriverObject);

	DisableDefence();

	DbgPrint("Unloaded.\n");
}

NTSTATUS EnableDefence() 
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

	if (result == STATUS_SUCCESS) 
	{
		DbgPrint("Callback registered.\n");
	}
	else 
	{
		DbgPrint("Callback registration fail: ");
		if (result == STATUS_FLT_INSTANCE_ALTITUDE_COLLISION) 
		{
			DbgPrint("Altitude collision.\n");
		}
		if (result == STATUS_INVALID_PARAMETER) 
		{
			DbgPrint("Invalid parameter.\n");
		}
		if (result == STATUS_ACCESS_DENIED) 
		{
			DbgPrint("The callback routines do not reside in a signed kernel binary image.\n");
		}
		if (result == STATUS_INSUFFICIENT_RESOURCES) 
		{
			DbgPrint("An attempt to allocate memory failed.\n");
		}		
	}

	return result;
}

NTSTATUS DisableDefence()
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
			DbgPrint("%s protected\n", ProcessNameStr);
		}
	}
	
	return OB_PREOP_SUCCESS;
}