#include <ntddk.h>
#include "Functions.h"
#include "ProcessDefender.h"
#include "Common.h"

NTSTATUS DriverDispatchIoctl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrint("DriverDispatchIoctl\n");
	NTSTATUS result = STATUS_SUCCESS;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG inputBufferLength = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (ControlCode == IOCTL_UPDATE_PROCESS_DEFENDER) 
	{
		if (inputBufferLength == sizeof(PROCESS_DEFENDER_OBJECT))
		{
			PPROCESS_DEFENDER_OBJECT ProcessDefenderObject = (PPROCESS_DEFENDER_OBJECT)(Irp->AssociatedIrp.SystemBuffer);
			UpdateProcessDefenderObject(ProcessDefenderObject);
		}
	}
	else {
		result = STATUS_NOT_SUPPORTED;
	}
	
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return result;
}

NTSTATUS DispatchCreate(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrint("DispatchCreate\n");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}