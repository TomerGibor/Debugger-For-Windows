#pragma once

#include <Windows.h>
#include <stdio.h>

#include "error.h"

extern LPVOID base_of_image;
extern STARTUPINFOA si;
extern PROCESS_INFORMATION pi;

error_t start_debugee(int argc, char* argv[]);
