#pragma once

#include <Windows.h>

#define DELIM (" ")
#define MAX_DATA_IN_COMMAND (3)
#define MAX_COMMAND_NAME_LEN (32)
#define MAX_COMMAND_DATA_LEN (16)
#define MAX_COMMAND_LEN (MAX_COMMAND_NAME_LEN + MAX_DATA_IN_COMMAND*MAX_COMMAND_DATA_LEN + (sizeof(DELIM)-1)*MAX_DATA_IN_COMMAND)

typedef enum command_data_type_e
{
	DATA_TYPE_NO_DATA = 0,
	DATA_TYPE_STRING = 1,
	DATA_TYPE_NUMBER = 2
} command_data_type_t;

typedef struct command_arg_s
{
	command_data_type_t type;

	union
	{
		char* string;
		ULONG64 number;
	} u;
} command_arg_t;

typedef struct command_s
{
	char* name;
	command_arg_t args[MAX_DATA_IN_COMMAND];
} command_t;
