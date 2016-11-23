#pragma once

#include <ntddk.h>

NTSTATUS ProcessDefenderDispatchIoctl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS ProcessDefenderDispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS ProcessDefenderDispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
