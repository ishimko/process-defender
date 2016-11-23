#pragma once

#include <ntddk.h>
#include "Common.h"

#define PROCESS_CREATE_PROCESS	(0x0080)
#define PROCESS_CREATE_THREAD	(0x0002)
#define PROCESS_DUP_HANDLE		(0x0040)
#define PROCESS_SET_QUOTA		(0x0100)
#define PROCESS_SET_INFORMATION (0x0200)
#define PROCESS_SUSPEND_RESUME	(0x0800)
#define PROCESS_TERMINATE		(0x0001)
#define PROCESS_VM_OPERATION	(0x0008)
#define PROCESS_VM_WRITE		(0x0020)

#define FORBIDDEN_OPERATIONS_COUNT 3

ACCESS_MASK FilterAccess(_In_ ACCESS_MASK OriginalDesiredAccess);
BOOLEAN HasOperation(ACCESS_MASK DesiredAccess, ACCESS_MASK OperationToCheck);
VOID ExcludeOperation(PACCESS_MASK DesiredAccess, ACCESS_MASK OperationToExclude);
BOOLEAN IsProtectedProcess(_In_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject, const STRING * ProcessName);

VOID InitializeProcessDefender(PPROCESS_DEFENDER_OBJECT ProcessDefenderObject);
VOID EnableProcessDefender(PPROCESS_DEFENDER_OBJECT ProcessDefenderObject);
VOID DisableProcessDefender(PPROCESS_DEFENDER_OBJECT ProcessDefenderObject);
VOID SetProcessName(PPROCESS_DEFENDER_OBJECT ProcessDefenderObject, const CHAR processName[MAX_PATH]);