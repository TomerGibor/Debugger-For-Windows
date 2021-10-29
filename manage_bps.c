#include "manage_bps.h"

#include <stdio.h>

bps_table_entry breakpoints[MAX_BPS] = {0};

void print_bp_table()
{
	int actives = 0;
	for (int i = 0; i < sizeof(breakpoints) / sizeof(bps_table_entry); i++)
	{
		if (breakpoints[i].active)
		{
			printf("Breakpoint %d at number %llu\n", i, breakpoints[i].offset);
			actives = 1;
		}
	}
	if (!actives)
		printf("No active breakpoints found\n");
}

int find_next_free_index()
{
	for (int i = 0; i < sizeof(breakpoints) / sizeof(bps_table_entry); i++)
	{
		if (!breakpoints[i].active)
			return i;
	}
	return -1;
}

int find_bp_by_offset(ULONG64 offset)
{
	for (int i = 0; i < sizeof(breakpoints) / sizeof(bps_table_entry); i++)
	{
		if (breakpoints[i].offset == offset)
			return i;
	}
	return -1;
}

void reset_bp_entry(int index)
{
	breakpoints[index].active = FALSE;
	breakpoints[index].offset = 0;
	breakpoints[index].read_byte = 0;
}

error_t decrease_rip()
{
	BOOL res = FALSE;
	CONTEXT context = {0};
	context.ContextFlags = CONTEXT_ALL;
	res = GetThreadContext(pi.hThread, &context);
	if (!res)
	{
		printf("GetThreadContext failed to obtain context with error_t: %lu\n", GetLastError());
		return ERROR_CONTEXT_FAILURE;
	}
	context.Rip--;
	res = SetThreadContext(pi.hThread, &context);
	if (!res)
	{
		printf("SetThreadContext failed to set context with error_t: %lu\n", GetLastError());
		return ERROR_CONTEXT_FAILURE;
	}
	return SUCCESS;
}

error_t insert_bp(ULONG64 offset)
{
	UINT8 read_byte = 0;
	BOOL res = FALSE;
	BYTE bp = BP;
	LPVOID addr = (LPVOID)((ULONG64)base_of_image + offset);
	int index = find_next_free_index();
	if (index < 0)
	{
		printf("Exceeded max breakpoints allowance!\n");
		return ERROR_MAX_BPS;
	}
	res = ReadProcessMemory(pi.hProcess, addr, &read_byte, 1, NULL);
	if (!res)
	{
		printf("ReadProcessMemory failed to read instruction with error_t: %lu\n", GetLastError());
		return ERROR_READ_WRITE_PROCESS;
	}
	res = WriteProcessMemory(pi.hProcess, addr, &bp, 1, NULL);
	if (!res)
	{
		printf("WriteProcessMemory failed to write bp with error_t: %lu\n", GetLastError());
		return ERROR_READ_WRITE_PROCESS;
	}
	FlushInstructionCache(pi.hProcess, addr, 1);
	breakpoints[index].active = TRUE;
	breakpoints[index].offset = offset;
	breakpoints[index].read_byte = read_byte;
	return SUCCESS;
}

error_t remove_bp(ULONG64 offset)
{
	int res = FALSE;
	LPVOID addr = (LPVOID)((ULONG64)base_of_image + offset);
	int index = find_bp_by_offset(offset);
	if (index < 0)
	{
		printf("No breakpoints is registered at that number!\n");
		return ERROR_NO_BP_AT_ADDRESS;
	}

	res = WriteProcessMemory(pi.hProcess, addr, &breakpoints[index].read_byte, 1, NULL);
	if (!res)
	{
		printf("WriteProcessMemory failed to write original instruction with error_t: %lu\n", GetLastError());
		return ERROR_READ_WRITE_PROCESS;
	}
	res = decrease_rip();
	if (res != SUCCESS)
	{
		printf("Decreasing of RIP failed with error_t %d\n", res);
		return res;
	}

	FlushInstructionCache(pi.hProcess, addr, 1);
	reset_bp_entry(index);
	return SUCCESS;
}
