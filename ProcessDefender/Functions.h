#pragma once

#include <ntddk.h>

NTSTATUS DriverDispatchIoctl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
