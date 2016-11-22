#include <ntddk.h>
#include "ProcessDefender.h"

ACCESS_MASK FORBIDDEN_OPERATIONS[FORBIDDEN_OPERATIONS_COUNT] = { PROCESS_TERMINATE, PROCESS_VM_WRITE, PROCESS_SET_INFORMATION };
STRING ProtectedProcess = RTL_CONSTANT_STRING("calc.exe");

ACCESS_MASK FilterAccess(_In_ ACCESS_MASK OriginalDesiredAccess) 
{
	ACCESS_MASK result = OriginalDesiredAccess;	
	
	for (int i = 0; i < FORBIDDEN_OPERATIONS_COUNT; i++) 
	{
		ACCESS_MASK operation = FORBIDDEN_OPERATIONS[i];
		if ((OriginalDesiredAccess & operation) == operation)
		{
			result &= ~operation;	
		}
	}

	return result;
}

BOOLEAN IsProtectedProcess(_In_ const STRING* ProcessName)
{
	return RtlEqualString(ProcessName, &ProtectedProcess, TRUE);
}