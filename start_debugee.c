#define _CRT_SECURE_NO_WARNINGS
#include "start_debugee.h"

LPVOID base_of_image = NULL;
STARTUPINFOA si = {.cb = sizeof(si)};
PROCESS_INFORMATION pi = {0};

error_t start_debugee(int argc, char* argv[])
{
	char proc_name[MAX_PATH] = {0};
	BOOL res = FALSE;

	if (argc == 2)
	{
		strncpy(proc_name, argv[1], MAX_PATH - 1);
	}
	else
	{
		printf("Enter path to a 64-bit executable: ");
		fgets(proc_name, MAX_PATH - 1, stdin);
		proc_name[strcspn(proc_name, "\n")] = 0;
	}
	res = CreateProcessA(proc_name, NULL, NULL, NULL,
	                     FALSE, DEBUG_PROCESS, NULL,
	                     NULL, &si, &pi);
	if (!res)
	{
		printf("OpenProcessA failed to open the executable with error_t: %lu\n", GetLastError());
		return ERROR_OPEN_PROCESS_FAILURE;
	}
	return SUCCESS;
}
