#include "handle_dbg_event.h"


#include "handle_commands.h"
#include "manage_bps.h"

error_t (*debug_event_handlers[MAX_EVENT_CODE])(LPDEBUG_EVENT, PBOOL) = {
	handle_exception_dbg_event,
	handle_create_thread_dbg_event,
	handle_create_process_dbg_event,
	handle_exit_thread_dbg_event,
	handle_exit_process_dbg_event,
	handle_load_dll_dbg_event,
	handle_unload_dll_dbg_event,
	handle_output_string_dbg_event,
	handle_rip_event_dbg_event
};

ULONG64 current_bp_address = 0;


error_t handle_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	if (1 <= debug_event->dwDebugEventCode && debug_event->dwDebugEventCode <= MAX_EVENT_CODE)
		return debug_event_handlers[debug_event->dwDebugEventCode - 1](debug_event, continue_event);
	return ERROR_INVALID_DEBUG_EVENT;
}

error_t handle_exception_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Exception debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	printf("Exception code: %#llx\n", debug_event->u.Exception.ExceptionRecord.ExceptionCode);
	printf("Exception address: %#llx\n", debug_event->u.Exception.ExceptionRecord.ExceptionAddress);

	*continue_event = FALSE;

	switch (debug_event->u.Exception.ExceptionRecord.ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
		current_bp_address = (ULONG64)debug_event->u.Exception.ExceptionRecord.ExceptionAddress;
		break;
	case EXCEPTION_SINGLE_STEP:
		if (re_insert_bp)
			insert_bp(current_bp_address - (ULONG64)base_of_image);

		re_insert_bp = FALSE;
		if (is_continue)
			*continue_event = TRUE;
		is_continue = FALSE;
		break;
	}
	return SUCCESS;
}

error_t handle_create_thread_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Create thread debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = FALSE;
	return SUCCESS;
}

error_t handle_create_process_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Create process debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	base_of_image = (LPVOID)debug_event->u.CreateProcessInfo.lpBaseOfImage;
	printf("Entry point is: %#llx\n", base_of_image);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_exit_thread_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Exit thread debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_exit_process_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Exit process debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_load_dll_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Load DLL debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_unload_dll_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Unload DLL debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_output_string_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("Output debug string event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}

error_t handle_rip_event_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event)
{
	printf("RIP event debug event triggered: event number %lu\n", debug_event->dwDebugEventCode);
	*continue_event = TRUE;
	return SUCCESS;
}
