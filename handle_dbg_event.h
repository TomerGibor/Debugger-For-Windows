#pragma once
#include <Windows.h>


#include "error.h"

#define MAX_EVENT_CODE (9)

extern ULONG64 current_bp_address;

error_t handle_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);

error_t handle_exception_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_create_thread_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_create_process_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_exit_thread_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_exit_process_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_load_dll_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_unload_dll_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_output_string_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
error_t handle_rip_event_dbg_event(LPDEBUG_EVENT debug_event, PBOOL continue_event);
