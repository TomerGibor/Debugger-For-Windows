#define _CRT_SECURE_NO_WARNINGS
#include "parse_commands.h"

void parse_data(char* text, command_t* command, int data_index)
{
	if (text[0] == '0' && text[1] == 'x')
	{
		// number in hex
		command->args[data_index].u.number = strtoll(text + 2, NULL, 16);
		command->args[data_index].type = DATA_TYPE_NUMBER;
	}
	else if ('0' <= text[0] && text[0] <= '9')
	{
		// number in base 10
		command->args[data_index].u.number = atoll(text);
		command->args[data_index].type = DATA_TYPE_NUMBER;
	}
	else
	{
		// string
		command->args[data_index].u.string = _strdup(text);
		command->args[data_index].type = DATA_TYPE_STRING;
	}
}

error_t parse_command(char* text, command_t* command)
{
	int i = 0;
	char* command_text = _strdup(text);
	char* token = strtok(command_text, DELIM);

	command->name = _strdup(token);

	while (token && i < MAX_DATA_IN_COMMAND)
	{
		token = strtok(NULL, DELIM);
		if (token)
			parse_data(token, command, i++);
	}

	free(command_text);
	return SUCCESS;
}

error_t free_command(command_t* cmnd)
{
	free(cmnd->name);
	for (int i = 0; i < MAX_COMMAND_DATA_LEN; i++)
	{
		if (cmnd->args[i].type == DATA_TYPE_STRING)
			free(cmnd->args[i].u.string);
	}
	memset(cmnd, 0, sizeof(command_t));
	return SUCCESS;
}
