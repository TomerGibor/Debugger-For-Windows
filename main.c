#include <stdio.h>
#include <Windows.h>

#include "error.h"
#include "handle_commands.h"
#include "handle_dbg_event.h"
#include "start_debugee.h"

int main(int argc, char* argv[])
{
	DEBUG_EVENT debug_event = {0};
	error_t res = SUCCESS;
	BOOL need_wait = FALSE;
	BOOL continue_event = FALSE;

	res = start_debugee(argc, argv);
	if (res != SUCCESS)
	{
		printf("Failed to start debugee!\n");
		return -1;
	}
	while (TRUE)
	{
		WaitForDebugEvent(&debug_event, INFINITE);
		handle_dbg_event(&debug_event, &continue_event);
		if (continue_event)
			ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
		else
		{
			while (!need_wait)
				take_command_from_user(&need_wait);
			need_wait = FALSE;
		}
	}
}
