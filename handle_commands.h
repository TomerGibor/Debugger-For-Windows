#pragma once

#include <stdio.h>
#include <string.h>
#include <Windows.h>


#include "error.h"
#include "command.h"

#define MAX_ALIASES (3)
#define STEPI_FLAG (0x100)
#define DEFAULT_INFO_STACK_NUM_BYTES (100)

typedef struct command_handler_table_entry_s
{
	const char* command_aliases[MAX_ALIASES];
	error_t (*handler)(command_t*, PBOOL);
} command_handler_table_entry_t;

extern BOOL running;
extern ULONG64 bp_address;


error_t handle_command(command_t* cmnd, PBOOL need_wait);

error_t handle_print(command_t* cmnd, PBOOL need_wait);
error_t handle_info(command_t* cmnd, PBOOL need_wait);
error_t handle_help(command_t* cmnd, PBOOL need_wait);
error_t handle_run(command_t* cmnd, PBOOL need_wait);
error_t handle_breakpoint(command_t* cmnd, PBOOL need_wait);
error_t handle_stepi(command_t* cmnd, PBOOL need_wait);
error_t handle_continue(command_t* cmnd, PBOOL need_wait);
error_t handle_delete(command_t* cmnd, PBOOL need_wait);
error_t handle_exit(command_t* cmnd, PBOOL need_wait);

error_t take_command_from_user(PBOOL need_wait);
