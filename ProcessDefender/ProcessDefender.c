#include <ntddk.h>
#include "ProcessDefender.h"
#include "Common.h"

ACCESS_MASK FilterAccess(_In_ ACCESS_MASK DesiredAccess) 
{
	static ACCESS_MASK FORBIDDEN_OPERATIONS[FORBIDDEN_OPERATIONS_COUNT] = {
		PROCESS_TERMINATE,
		PROCESS_VM_WRITE,
		PROCESS_SET_INFORMATION
	};
	
	for (int i = 0; i < FORBIDDEN_OPERATIONS_COUNT; i++) 
	{
		ACCESS_MASK operation = FORBIDDEN_OPERATIONS[i];
		if (HasOperation(DesiredAccess, operation))
		{
			ExcludeOperation(&DesiredAccess, operation);
		}
	}

	return DesiredAccess;
}

inline BOOLEAN HasOperation(_In_ ACCESS_MASK DesiredAccess, _In_ ACCESS_MASK OperationToCheck) 
{
	return (DesiredAccess & OperationToCheck) == OperationToCheck;
}

inline VOID ExcludeOperation(_Inout_ PACCESS_MASK DesiredAccess, _In_ ACCESS_MASK OperationToExclude)
{
	*DesiredAccess &= ~OperationToExclude;
}

BOOLEAN IsProtectedProcess(_In_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject, _In_ const STRING* ProcessName)
{
	BOOLEAN result = FALSE;

	if (ProcessDefenderObject->enabled) {
		STRING ProtectedProcess;
		RtlInitString(&ProtectedProcess, ProcessDefenderObject->protectedProcess);
		result = RtlEqualString(ProcessName, &ProtectedProcess, TRUE);
	}

	return result;
}

VOID InitializeProcessDefender(_Inout_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject)
{
	RtlZeroMemory(ProcessDefenderObject, sizeof(ProcessDefenderObject));
}

VOID EnableProcessDefender(_Inout_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject)
{
	ProcessDefenderObject->enabled = TRUE;
}

VOID DisableProcessDefender(_Inout_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject)
{
	ProcessDefenderObject->enabled = FALSE;
}

VOID SetProcessName(_Inout_ PPROCESS_DEFENDER_OBJECT ProcessDefenderObject, _In_ const CHAR processName[MAX_PATH])
{
	RtlCopyMemory(ProcessDefenderObject->protectedProcess, processName, sizeof(ProcessDefenderObject->protectedProcess));
}