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


#include <stdint.h>
#include <stddef.h>

#include <string.h>
#include <stdlib.h>


// Process IDs
#define KB_I_PROCESS_ID        0
#define CRT_I_PROCESS_ID       1
#define P_PROCESS_ID 		   2

// RTX Constants
#define MSG_ENV_SIZE 1024
#define MSG_ENV_COUNT 100
#define NUM_PRIORITY_LEVEL 5
#define PROCESS_COUNT 3
#define STACK_SIZE 100

// error codes
#define SUCCESS 0
#define NULL_ARGUMENT -1
#define ILLEGAL_ARGUMENT -2
#define OTHER_ERROR -3

typedef int bool;
#define TRUE 1
#define FALSE 0

#define DEBUG 1

typedef enum msg_type {
    CONSOLE_INPUT, CONSOLE_OUTPUT, DISPLAY_ACK, GET_CHAR, COUNT_REPORT, WAKEUP10
}MsgType;	int priority;

typedef struct MsgEnv {
   struct MsgEnv *next;
    int 	dest_pid;
    int 	sender_pid;
    MsgType 	msg_type;
    char    *data;
} MsgEnv;

typedef enum process_states {
    READY, EXECUTING, BLOCKED_ON_ENV_REQUEST, BLOCKED_ON_RCV, NO_BLK_RCV,
    SINTERRUPTED
} ProcessState;

typedef struct msg_env_queue {
	MsgEnv* head;
	MsgEnv* tail;
}MsgEnvQ;



typedef struct process_control_block {
	int pid;
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
pcb* current_process;
pcb* prev_process;
MsgEnvQ* free_env_queue;
pcb* pcb_list[PROCESS_COUNT];

// Initialization Table
InitProc init_table[PROCESS_COUNT] = {

};

#endif

