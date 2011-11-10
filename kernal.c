// kernal.c
#include "rtx.h"
#include "kernal.h"
#include <signal.h>

pcb* pid_to_pcb(int pid)
{
	switch (pid) {

		case 0 : return pcb_list[0];
		case 1 : return pcb_list[1];
		case 3 : return pcb_list[3];
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
	MsgEnvQ_enqueue(free_env_queue,env);
	// check processes blocked for allocate envelope
	return SUCCESS;
}

int k_send_message(int dest_process_id, MsgEnv *msg_envelope)
{
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);
	if (!dest_pcb || !msg_envelope)
		return NULL_ARGUMENT;

	msg_envelope->sender_pid = current_process->pid;
	msg_envelope->dest_pid = dest_process_id;

	MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);
	printf("message SENT on enqueued on PID %i\n",dest_pcb->pid);
	return SUCCESS;
}

MsgEnv* k_receive_message()
{
	MsgEnv* ret = NULL;
	printf("Current PCB msgQ size is %i\n", MsgEnvQ_size(current_process->rcv_msg_queue) );
	if (MsgEnvQ_size(current_process->rcv_msg_queue) > 0){
		ret = MsgEnvQ_dequeue(current_process->rcv_msg_queue);
		printf("%i",ret->dest_pid);
	}


	printf("Sender PID %i\n",ret->sender_pid);

	return ret;
}

int k_send_console_chars(MsgEnv *message_envelope)
{
	if (!message_envelope)
		return NULL_ARGUMENT;
	message_envelope->msg_type == DISPLAY_ACK;
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

	current_process = pid_to_pcb(KB_I_PROCESS_ID);
	printf("got here\n");
	kbd_i_proc(0);
	//kill(getpid(),SIGUSR1);
	printf("keyboard process returned to get-console-chars\n");

	return retVal;
}

void atomic(bool state)
{
	if (state != TRUE || state!= FALSE)
		return;

	static sigset_t oldmask;
	sigset_t newmask;
	if (state == TRUE)
	{
		current_process->a_count ++; //every time a primitive is called, increment by 1
		//mask the signals, so atomicity is enforced
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGALRM); //the alarm signal
		sigaddset(&newmask, SIGINT); // the CNTRL-C
		sigaddset(&newmask, SIGUSR1); // the KB signal
		sigaddset(&newmask, SIGUSR2); // the CRT signal
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);
	}
	else
	{
		current_process->a_count--; //every time a primitive finishes, decrement by 1
		//if all primitives completes, restore old mask, allow signals
		if (current_process->a_count == 0)
		{
			//restore old mask
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}
	}
}

