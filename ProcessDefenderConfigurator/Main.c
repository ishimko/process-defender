#include <Windows.h>
#include <stdio.h>
#include <cstdbool>
#include "Common.h"

#define MAX_PARAMS_COUNT 3
#define MIN_PARAMS_COUNT 2

#define ENABLE_PARAM "-e"
#define DISABLE_PARAM "-d"

void DisplayHelp();
bool EnableProcessDefender(char processName[MAX_PATH]);
bool DisableProcessDefender();
bool SendToDriver(PPROCESS_DEFENDER_OBJECT pProcessDefenderObject);

int main(int argc, char *argv[])
{
	int result = 0;
	if ((argc > MAX_PARAMS_COUNT) || (argc < MIN_PARAMS_COUNT)) {
		DisplayHelp();
		result = 1;
	}
	else {
		char* commandParam = argv[1];
		if (strcmp(commandParam, ENABLE_PARAM) == 0) {
			if (argc == MAX_PARAMS_COUNT) {
				char* processName = argv[2];
				if (strlen(processName) <= MAX_PATH) {
					result = EnableProcessDefender(processName) ? 0 : 1;
					if (result == 0) {
						puts("Enable request sent.");
					}
				}
				else {
					fprintf(stderr, "Too long process name.\n");
					result = 1;
				}
			}
			else {
				DisplayHelp();
			}
		}
		else if (strcmp(commandParam, DISABLE_PARAM) == 0) {
			result = DisableProcessDefender() ? 0 : 1;
			if (result == 0) {
				puts("Disable request sent.");
			}
		}
		else {
			DisplayHelp();
			result = 1;
		}
	}
	
	return result;
}

void DisplayHelp()
{
	puts("Usage:");
	puts("\t"ENABLE_PARAM" <process_name> - enable defender for specified process name");
	puts("\t"DISABLE_PARAM" - disable defender");
}

bool EnableProcessDefender(char processName[MAX_PATH])
{
	PROCESS_DEFENDER_OBJECT processDefenderObject;
	memset(&processDefenderObject, 0, sizeof(PROCESS_DEFENDER_OBJECT));
	processDefenderObject.enabled= TRUE;
	strcpy_s(processDefenderObject.protectedProcess, sizeof(processDefenderObject.protectedProcess), processName);
	
	return SendToDriver(&processDefenderObject);
}

bool DisableProcessDefender()
{
	PROCESS_DEFENDER_OBJECT processDefenderObject;
	memset(&processDefenderObject, 0, sizeof(PROCESS_DEFENDER_OBJECT));
	processDefenderObject.enabled= FALSE;

	return SendToDriver(&processDefenderObject);
}

bool SendToDriver(PPROCESS_DEFENDER_OBJECT pProcessDefenderObject)
{
	bool result = true;

	HANDLE controlDriver = CreateFile(L"\\\\.\\"DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (controlDriver != INVALID_HANDLE_VALUE)
	{
		DWORD bytesReturned;
		if (!DeviceIoControl(controlDriver, IOCTL_UPDATE_PROCESS_DEFENDER, pProcessDefenderObject, sizeof(*pProcessDefenderObject), NULL, 0, &bytesReturned, NULL)) {
			fprintf(stderr, "DeviceIoControl failed. Error %d\n", GetLastError());
			result = false;
		}

		CloseHandle(controlDriver);
	} else {
		fprintf(stderr, "CreateFile failed. Error %d\n", GetLastError());
		result = false;
	}

	return result;
}