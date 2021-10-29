#pragma once
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "command.h"

error_t parse_command(char* text, command_t* command);

error_t free_command(command_t* command);
