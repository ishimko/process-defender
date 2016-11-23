#include <Windows.h>
#include <stdio.h>
#include "Common.h"

int main()
{
	HANDLE controlDriver = CreateFile(L"\\\\.\\"DRIVER_NAME, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	PROCESS_DEFENDER_OBJECT processDefenderObject;
	memset(&processDefenderObject, 0, sizeof(processDefenderObject));
	processDefenderObject.isActive = TRUE;
	strcpy_s(processDefenderObject.protectedProcess, strlen("notepad.exe"), "notepad.exe");

	if (controlDriver != INVALID_HANDLE_VALUE)
	{
		puts("CreateFile success");
		DWORD bytesReturned;
		if (!DeviceIoControl(controlDriver, IOCTL_UPDATE_PROCESS_DEFENDER, &processDefenderObject, sizeof(processDefenderObject), NULL, 0, &bytesReturned, NULL)) {
			printf("DeviceIoControl failed. Error %d\n", GetLastError());
		}
		else {
			puts("DeviceIoControl success");
		};
		CloseHandle(controlDriver);
		puts("CloseHandle success");
	} else {
		printf("CreateFile failed. Error %d\n", GetLastError());
	}

	getchar();
}