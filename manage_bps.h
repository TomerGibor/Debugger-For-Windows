#pragma once
#include <Windows.h>

#include "error.h"
#include "start_debugee.h"
#define MAX_BPS 10
#define BP 0xCC

typedef struct bps_table_entry
{
	BOOL active;
	ULONG64 offset;
	UINT8 read_byte;
} bps_table_entry;

void print_bp_table();
error_t insert_bp(ULONG64 offset);
error_t remove_bp(ULONG64 offset);
