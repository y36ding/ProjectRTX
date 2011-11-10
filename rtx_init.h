#ifndef _RTX_INIT_
#define _RTX_INIT_

#include "rtx.h"

typedef struct init_proc
{
	char* name;
	int pid;
	int priority;
	bool is_i_process;
	int stack_size;
	void* address;
}InitialProcess;

extern InitialProcess init_table[PROCESS_COUNT];

void rtx_init();

#endif
