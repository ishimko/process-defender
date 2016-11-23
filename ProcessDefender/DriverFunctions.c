#include <ntddk.h>
#include "DriverFunctions.h"
#include "ProcessDefender.h"
#include "Common.h"
#include "Driver.h"

NTSTATUS ProcessDefenderDispatchIoctl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrint(DBG_PREFIX"IOCTL received\n");
	NTSTATUS result = STATUS_SUCCESS;
	PPROCESS_DEFENDER_OBJECT processDefenderObject = &(GetDriverData()->ProcessDefenderObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG inputBufferLength = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (controlCode == IOCTL_UPDATE_PROCESS_DEFENDER) 
	{
		if (inputBufferLength == sizeof(PROCESS_DEFENDER_OBJECT))
		{
			PPROCESS_DEFENDER_OBJECT receivedProcessDefenderObject = (PPROCESS_DEFENDER_OBJECT)(Irp->AssociatedIrp.SystemBuffer);
			if (receivedProcessDefenderObject->enabled) {
				DbgPrint(DBG_PREFIX"enable defender\n");
				EnableProcessDefender(processDefenderObject);
				SetProcessName(processDefenderObject, receivedProcessDefenderObject->protectedProcess);
			}
			else {
				DbgPrint(DBG_PREFIX"disable defender\n");
				DisableProcessDefender(processDefenderObject);
			}
		}
	}
	else {
		result = STATUS_NOT_SUPPORTED;
	}
	
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return result;
}

NTSTATUS ProcessDefenderDispatchCreate(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrint(DBG_PREFIX"create\n");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS ProcessDefenderDispatchClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrint(DBG_PREFIX"close\n");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}