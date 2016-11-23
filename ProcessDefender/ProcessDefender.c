#include <ntddk.h>
#include "ProcessDefender.h"
#include "Common.h"

ACCESS_MASK FORBIDDEN_OPERATIONS[FORBIDDEN_OPERATIONS_COUNT] = { PROCESS_TERMINATE, PROCESS_VM_WRITE, PROCESS_SET_INFORMATION };
//STRING ProtectedProcess = RTL_CONSTANT_STRING("calc.exe");
PROCESS_DEFENDER_OBJECT ProcessDefenderObject;

ACCESS_MASK FilterAccess(_In_ ACCESS_MASK OriginalDesiredAccess) 
{
	ACCESS_MASK result = OriginalDesiredAccess;	
	
	for (int i = 0; i < FORBIDDEN_OPERATIONS_COUNT; i++) 
	{
		ACCESS_MASK operation = FORBIDDEN_OPERATIONS[i];
		if (HasOperation(OriginalDesiredAccess, operation))
		{
			result = ExcludeOperation(OriginalDesiredAccess, operation);
		}
	}

	return result;
}

inline BOOLEAN HasOperation(ACCESS_MASK DesiredAccess, ACCESS_MASK OperationToCheck) 
{
	return (DesiredAccess & OperationToCheck) == OperationToCheck;
}

inline ACCESS_MASK ExcludeOperation(ACCESS_MASK OriginalDesiredAccess, ACCESS_MASK OperationToExclude)
{
	return (OriginalDesiredAccess & (~OperationToExclude));
}

BOOLEAN IsProtectedProcess(_In_ const STRING* ProcessName)
{
	BOOLEAN result = FALSE;

	if (ProcessDefenderObject.isActive) {
		STRING ProtectedProcess;
		RtlInitString(&ProtectedProcess, ProcessDefenderObject.protectedProcess);
		result = RtlEqualString(ProcessName, &ProtectedProcess, TRUE);
	}

	return result;
}

VOID InitializeProcessDefender()
{
	RtlZeroMemory(&ProcessDefenderObject, sizeof(ProcessDefenderObject));
}

VOID UpdateProcessDefenderObject(_In_ PPROCESS_DEFENDER_OBJECT NewProcessDefenderObject)
{
	RtlCopyMemory(&ProcessDefenderObject, NewProcessDefenderObject, sizeof(ProcessDefenderObject));
}