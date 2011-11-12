#ifndef _RTX_HEADER_
#define _RTX_HEADER_

#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "iProcs.h"
#include "kbcrt.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


// Process IDs
#define KB_I_PROCESS_ID 	0
#define CRT_I_PROCESS_ID	1
#define P_PROCESS_ID 		2

// RTX Constants
#define MSG_ENV_SIZE 		100
#define MSG_ENV_COUNT 		50
#define NUM_PRIORITY_LEVEL 	5
#define PROCESS_COUNT 		3
#define STACK_SIZE 			100

// error codes
#define SUCCESS 			0
#define NULL_ARGUMENT 		-1
#define ILLEGAL_ARGUMENT 	-2
#define OTHER_ERROR 		-3

typedef int bool;
#define TRUE 		1
#define FALSE 		0

// atomic flags
#define ON TRUE
#define OFF FALSE

#define DEBUG 		0

void die(int signal);

typedef enum msg_type {
    CONSOLE_INPUT, DISPLAY_ACK, COUNT_REPORT, WAKEUP10
}MsgType;

typedef struct MsgEnv {
   struct MsgEnv *next;
    int 	dest_pid;
    int 	sender_pid;
    MsgType 	msg_type;
    char    *data;
} MsgEnv;

typedef enum process_states {
    READY, EXECUTING, BLOCKED_ON_ENV_REQUEST, BLOCKED_ON_RCV, NEVER_BLK_RCV,
    SINTERRUPTED
} ProcessState;

typedef struct msg_env_queue {
	MsgEnv* head;
	MsgEnv* tail;
}MsgEnvQ;

typedef struct process_control_block {
	int pid;
	int priority;
	ProcessState state;
	char* name;
	MsgEnvQ*  rcv_msg_queue;
	struct process_control_block* next;
	int a_count;
	bool is_i_process;
} pcb;

typedef struct init_proc
{
	char* name;
	int pid;
	int priority;
	bool is_i_process;
}InitProc;

// global variables
extern pcb* current_process;
extern pcb* prev_process;
extern MsgEnvQ* free_env_queue;
extern pcb* pcb_list[PROCESS_COUNT];
extern MsgEnvQ* displayQ;

// globals
extern inputbuf * in_mem_p_key;	// pointer to structure that is the shared memory
extern outputbuf* in_mem_p_crt;	// pointer to structure that is the shared memory
extern int in_pid_keyboard;		// pid of keyboard child process
extern int in_pid_crt;				// pid of crt child process
extern caddr_t mmap_ptr_keyboard;
extern caddr_t mmap_ptr_crt;
extern int bufsize;
extern int fid, fid2, status;		//used to create the shared memory

extern int numOfTicks;
extern int displayClock;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern char * sfilename;
extern char * cfilename;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif

