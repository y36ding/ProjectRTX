#include "rtx_init.h"


// Initialization Table
InitProc init_table[PROCESS_COUNT] = {
		{"Keyboard I proc\0", KB_I_PROCESS_ID, 0, TRUE},
		{"CRT I proc\0", CRT_I_PROCESS_ID, TRUE},
		{"P Process\0", P_PROCESS_ID, FALSE}
};

int init_all_lists()
{
	int i;
	int init_status = SUCCESS;

	for (i = 0; i < PROCESS_COUNT; ++i)
	{
		pcb_list[i] = (pcb*)malloc(sizeof(pcb));
		if (pcb_list[i] == NULL)
		{
			init_status   = OTHER_ERROR;
			break;
		}
		pcb_list[i]->pid = init_table[i].pid;
		pcb_list[i]->priority = init_table[i].priority;
		pcb_list[i]->state = READY;
		pcb_list[i]->name = init_table[i].name;
		pcb_list[i]->rcv_msg_queue = MsgEnvQ_create();
		pcb_list[i]->is_i_process = init_table[i].is_i_process;
	}

	for (i = 0; i < MSG_ENV_COUNT; i++)
	{
		msg_list[i] = (MsgEnv*)malloc(sizeof(MsgEnv));
		if (msg_list[i] == NULL)
		{
			init_status   = OTHER_ERROR;
			break;
		}
		msg_list[i]->data = (char*)malloc(sizeof(char)*MSG_ENV_SIZE);
		if (msg_list[i]->data == NULL)
		{
			init_status  = OTHER_ERROR;
			break;
		}
		MsgEnvQ_enqueue(free_env_queue, msg_list[i]);
		// deal with non null later
	}
	return init_status ;
}
