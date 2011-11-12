// kernal.c
#include "rtx.h"
#include "kernal.h"
#include <signal.h>
#include "MsgEnvQueue.h"

pcb* pid_to_pcb(int pid)
{
	switch (pid) {
		case 0 : return pcb_list[0];
		case 1 : return pcb_list[1];
		case 2 : return pcb_list[2];
		default: return NULL;
	}
}

MsgEnv* k_request_msg_env()
{
	// the real code will keep on trying to search free env
	// queue for envelope and get blocked otherwise
	if (MsgEnvQ_size(free_env_queue) == 0)
		return NULL;

	MsgEnv* free_env = MsgEnvQ_dequeue(free_env_queue);
	return free_env;
}

int k_release_message_env(MsgEnv* env)
{
	if (env == NULL)
		return NULL_ARGUMENT;
	MsgEnvQ_enqueue(free_env_queue, env);
	// check processes blocked for allocate envelope later
	return SUCCESS;
}

int k_send_message(int dest_process_id, MsgEnv *msg_envelope)
{
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);

	if (!dest_pcb || !msg_envelope) {
		printf("The destPCB or MSG_ENV is empty\n");
		return NULL_ARGUMENT;
	}

	msg_envelope->sender_pid = current_process->pid;
	msg_envelope->dest_pid = dest_process_id;

	MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);
	return SUCCESS;
}

MsgEnv* k_receive_message()
{
	MsgEnv* ret = NULL;

	if (MsgEnvQ_size(current_process->rcv_msg_queue) > 0){
		ret = MsgEnvQ_dequeue(current_process->rcv_msg_queue);
	}else{
		if (current_process->is_i_process == TRUE || current_process->state == NEVER_BLK_RCV)
			return ret;
	}

	return ret;
}

int k_send_console_chars(MsgEnv *message_envelope)
{
	if (!message_envelope)
		return NULL_ARGUMENT;

	message_envelope->msg_type = DISPLAY_ACK;
	int retVal = k_send_message(CRT_I_PROCESS_ID, message_envelope);
	crt_i_proc(0);
	return retVal;
}

int k_get_console_chars(MsgEnv *message_envelope)
{
	if (!message_envelope)
		return NULL_ARGUMENT;
	message_envelope->msg_type = CONSOLE_INPUT;
	int retVal = k_send_message( KB_I_PROCESS_ID, message_envelope);

	return retVal;
}

void atomic(bool state)
{

	static sigset_t oldmask;
	sigset_t newmask;
	if (state == TRUE)
	{
		//increment the count when a primitive executes
		//create a new mask to mask out the signals
		current_process->a_count ++;
		if (current_process->a_count == 1)
		{
			sigemptyset(&newmask);
			sigaddset(&newmask, SIGALRM); 
			sigaddset(&newmask, SIGINT); 
			sigaddset(&newmask, SIGUSR1);
			sigaddset(&newmask, SIGUSR2); 
			//sigprocmask(SIG_BLOCK, &newmask, &oldmask);
			sigprocmask(SIG_BLOCK, &newmask, NULL);
		}
	}
	else
	{
		//decrement the count when a primitive finishes
		current_process->a_count--; 

		//when all primitives finish, replace the new mask with the old one
		if (current_process->a_count == 0)
		{
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}
	}
}

int k_pseudo_process_switch(int pid)
{
	pcb* p = (pcb*)pid_to_pcb(pid);
	if (p == NULL)
		return ILLEGAL_ARGUMENT;
	prev_process = current_process;
	current_process = p;
	return SUCCESS;
}

void k_return_from_switch()
{
	pcb* temp = current_process;
	current_process = prev_process;
	prev_process = current_process;
}


