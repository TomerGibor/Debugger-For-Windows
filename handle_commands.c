#include "handle_commands.h"


#include "handle_dbg_event.h"
#include "manage_bps.h"
#include "parse_commands.h"
#include "start_debugee.h"

BOOL running = FALSE;
BOOL re_insert_bp = FALSE;

command_handler_table_entry_t handlers[] = {
	{{"info", "i", NULL}, handle_info},
	{{"help", "h", NULL}, handle_help},
	{{"run", "r", NULL}, handle_run},
	{{"breakpoint", "bp", "b"}, handle_breakpoint},
	{{"p", "print", NULL}, handle_print},
	{{"re_insert_bp", NULL, NULL}, handle_stepi},
	{{"continue", "c", NULL}, handle_continue},
	{{"delete", "d", NULL}, handle_delete},
	{{"exit", "quit", NULL}, handle_exit},
};

error_t handle_command(command_t* cmnd, PBOOL need_wait)
{
	if (!cmnd->name)
		return ERROR_INVALID_COMMAND_NAME;
	for (int i = 0; i < sizeof(handlers) / sizeof(command_handler_table_entry_t); i++)
	{
		for (int j = 0; j < MAX_ALIASES; j++)
		{
			if (handlers[i].command_aliases[j] &&
				!strncmp(handlers[i].command_aliases[j], cmnd->name, sizeof(handlers[i].command_aliases[j])))
			{
				return handlers[i].handler(cmnd, need_wait);
			}
		}
	}
	printf("Invalid command name: \"%s\" is not a registered command or alias.\n", cmnd->name);
	return ERROR_INVALID_COMMAND_NAME;
}

static error_t print_str(ULONG64 address)
{
	BOOL res = FALSE;
	unsigned char c = 1;
	int count = 0;
	while (c != 0 && count < 1000)
	{
		res = ReadProcessMemory(pi.hProcess, (void*)address, &c, 1, NULL);
		if (!res)
		{
			printf("ReadProcessMemory failed to read memory with error: %lu\n", GetLastError());
			return ERROR_READ_WRITE_PROCESS;
		}
		printf("%c", c);
		count++;
	}
	printf("\n");
	return SUCCESS;
}

static error_t print_bytes_from_address(ULONG64 address, ULONG64 num_bytes)
{
	int i = 0;
	ULONG64 read_bytes = 0;
	BOOL res = FALSE;
	if (num_bytes < 1)
		return ERROR_INVALID_COMMAND_ARGUMENT;
	BYTE* buf = (BYTE*)malloc(num_bytes + 1);

	res = ReadProcessMemory(pi.hProcess, (void*)address, buf, num_bytes, &read_bytes);
	if (!res || read_bytes != num_bytes)
	{
		printf("ReadProcessMemory failed to read memory with error: %lu\n", GetLastError());
		free(buf);
		return ERROR_READ_WRITE_PROCESS;
	}

	for (; i < num_bytes; i++)
	{
		if (i && i % 4 == 0)
			printf(" ");
		printf("%02x", buf[i]);
	}

	printf("\n");
	free(buf);
	return SUCCESS;
}

error_t handle_print(command_t* cmnd, PBOOL need_wait)
{
	*need_wait = FALSE;
	if (cmnd->args[0].type == DATA_TYPE_NUMBER)
	{
		if (!cmnd->args[0].u.number || cmnd->args[1].type != DATA_TYPE_NUMBER || !cmnd->args[1].u.number)
			return ERROR_INVALID_COMMAND_ARGUMENT;
		return print_bytes_from_address(cmnd->args[0].u.number, cmnd->args[1].u.number);
	}
	else if (cmnd->args[0].type == DATA_TYPE_STRING)
	{
		if (!strncmp("str", cmnd->args[0].u.string, sizeof("str")))
		{
			return print_str(cmnd->args[1].u.number);
		}
	}
	return ERROR_INVALID_COMMAND_ARGUMENT;
}

static error_t handle_info_reg()
{
	BOOL res = FALSE;
	CONTEXT context = {0};
	context.ContextFlags = CONTEXT_ALL;
	res = GetThreadContext(pi.hThread, &context);
	if (!res)
	{
		printf("GetThreadContext failed to obtain context with error: %lu\n", GetLastError());
		return ERROR_CONTEXT_FAILURE;
	}
	printf("RAX = %#16llx\nRBX = %#16llx\nRCX = %#16llx\n"
	       "RDX = %#16llx\nRSI = %#16llx\nRDI = %#16llx\n"
	       "RIP = %#16llx\nRSP = %#16llx\nRBP = %#16llx\n"
	       "EFlags =      %#08lx\n",
	       context.Rax, context.Rbx, context.Rcx,
	       context.Rdx, context.Rsi, context.Rdi,
	       context.Rip, context.Rsp, context.Rbp,
	       context.EFlags);
	return SUCCESS;
}

static error_t handle_info_b()
{
	print_bp_table();
	return SUCCESS;
}

static error_t handle_info_stack(ULONG64 num_bytes)
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
	return print_bytes_from_address(context.Rsp, num_bytes);
}

error_t handle_info(command_t* cmnd, PBOOL need_wait)
{
	if (!cmnd->args[0].u.string || cmnd->args[0].type != DATA_TYPE_STRING)
		return ERROR_INVALID_COMMAND_ARGUMENT;
	*need_wait = FALSE;
	if (!strncmp("reg", cmnd->args[0].u.string, sizeof("reg")))
	{
		return handle_info_reg();
	}
	else if (!strncmp("b", cmnd->args[0].u.string, sizeof("b")))
	{
		return handle_info_b();
	}
	else if (!strncmp("stack", cmnd->args[0].u.string, sizeof("stack")))
	{
		return handle_info_stack(cmnd->args[1].u.number ? cmnd->args[1].u.number : DEFAULT_INFO_STACK_NUM_BYTES);
	}
	return ERROR_INVALID_COMMAND_ARGUMENT;
}

error_t handle_help(command_t* cmnd, PBOOL need_wait)
{
	*need_wait = FALSE;
	printf(
		"Available Commands:\n"
		"\t- help | h - prints out the available commands.\n"
		"\t- info reg | i reg - prints out the registers' values.\n"
		"\t- info b | i b - prints out the current breakpoints with their indices.\n"
		"\t- info stack [<num_bytes>] | i stack [<num_bytes>] - prints out the bytes currently present in the stack starting from rsp and going into higher addresses. Defaults to 100 bytes, user may also decide how many bytes to print.\n"
		"\t- print <address> <num_bytes> | p <address> <num_bytes> - prints out the `num_bytes` bytes starting from `address`.\n"
		"\t- print str <address> | p str <address> - prints out the ascii chars starting from `address`, going on until reaching a \\0, or 1000 chars have been printed.\n"
		"\t- run | r - runs the executable.\n"
		//TODO
		"\n"
	);
	return SUCCESS;
}

error_t handle_run(command_t* cmnd, PBOOL need_wait)
{
	char run_again = 0;
	CONTEXT context = {0};

	if (!running)
	{
		*need_wait = TRUE;
		ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
		running = TRUE;
	}
	else
	{
		printf("The debugee is already running!\n");
		*need_wait = FALSE;
	}
	return SUCCESS;
}

error_t handle_breakpoint(command_t* cmnd, PBOOL need_wait)
{
	*need_wait = FALSE;
	if (!cmnd->args[0].u.number)
		return ERROR_INVALID_COMMAND_ARGUMENT;
	if (!insert_bp(cmnd->args[0].u.number))
	{
		printf("Successfully inserted breakpoint at number %llu\n", cmnd->args[0].u.number);
		printf("Breakpoint address: %#llx\n", cmnd->args[0].u.number + (DWORD64)base_of_image);
		return SUCCESS;
	}
	return ERROR_HANDLE_COMMAND_FAILURE;
}

error_t handle_stepi(command_t* cmnd, PBOOL need_wait)
{
	CONTEXT context = {0};
	context.ContextFlags = CONTEXT_ALL;
	if (!GetThreadContext(pi.hThread, &context))
	{
		printf("GetThreadContext failed to obtain context with error: %lu\n", GetLastError());
		return ERROR_CONTEXT_FAILURE;
	}
	context.EFlags |= STEPI_FLAG; // set trap flag, which raises "single-step" exception
	if (context.Rip - 1 == current_bp_address) // currently at breakpoint
	{
		context.Rip--;
		if (!remove_bp(context.Rip - (DWORD64)base_of_image))
			return ERROR_HANDLE_COMMAND_FAILURE;
		re_insert_bp = TRUE;
	}
	SetThreadContext(pi.hThread, &context);
	*need_wait = TRUE;
	ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
	return SUCCESS;
}

error_t handle_continue(command_t* cmnd, PBOOL need_wait)
{
	CONTEXT context = {0};
	context.ContextFlags = CONTEXT_ALL;
	if (!GetThreadContext(pi.hThread, &context))
	{
		printf("GetThreadContext failed to obtain context with error: %lu\n", GetLastError());
		return ERROR_CONTEXT_FAILURE;
	}
	if (context.Rip - 1 != current_bp_address) // already stepi-ed out of breakpoint
	{
		current_bp_address = 0;
		ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
		return SUCCESS;
	}

	context.EFlags |= STEPI_FLAG; // set trap flag, which raises "single-step" exception
	context.Rip--;
	if (!remove_bp(context.Rip - (DWORD64)base_of_image))
		return ERROR_HANDLE_COMMAND_FAILURE;
	SetThreadContext(pi.hThread, &context);
	*need_wait = TRUE;
	current_bp_address = 0;
	re_insert_bp = TRUE;
	ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
	return SUCCESS;
}

error_t handle_delete(command_t* cmnd, PBOOL need_wait)
{
	*need_wait = FALSE;
	if (!cmnd->args[0].u.number)
		return ERROR_INVALID_COMMAND_ARGUMENT;
	if (!remove_bp(cmnd->args[0].u.number))
	{
		printf("Successfully removed breakpoint at number %llu\n", cmnd->args[0].u.number);
		return SUCCESS;
	}
	return ERROR_HANDLE_COMMAND_FAILURE;
}

error_t handle_exit(command_t* cmnd, PBOOL need_wait)
{
	printf("Thanks for using my debugger! Have a pleasant day!\n");
	free_command(cmnd);
	exit(0);
}

error_t take_command_from_user(PBOOL need_wait)
{
	command_t cmnd = {0};
	char buffer[MAX_COMMAND_LEN] = {0};
	error_t res = SUCCESS;

	printf("dbg $ ");
	fgets(buffer, MAX_COMMAND_LEN - 1, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
	parse_command(buffer, &cmnd);
	res = handle_command(&cmnd, need_wait);
	if (res != SUCCESS)
	{
		printf("Command handling failed with error %d\n", res);
		res = ERROR_HANDLE_COMMAND_FAILURE;
	}
	free_command(&cmnd);
	return res;
}
