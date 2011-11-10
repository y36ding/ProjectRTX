// kernal.c
#include "rtx.h"
#include "kernal.h"
#include <signal.h>

pcb* pid_to_pcb(int pid)
{
	return NULL;
}

int k_send_message(int dest_process_id, MsgEnv *msg_envelope)
{
	pcb* dest_pcb =  pid_to_pcb(dest_process_id);
	if (!dest_pcb || !msg_envelope)
		return NULL_ARGUMENT;

	msg_envelope->sender_pid = current_process->pid;
	msg_envelope->dest_pid = dest_process_id;

	MsgEnvQ_enqueue(dest_pcb->rcv_msg_queue, msg_envelope);
	return SUCCESS;
}

MsgEnv* k_receive_message()
{
	// if i-process
	if (current_process->is_i_process == TRUE)
		return NULL;

	MsgEnv* ret = NULL;

	if (MsgEnvQ_size(current_process->rcv_msg_queue) > 0)
		ret = MsgEnvQ_dequeue(current_process->rcv_msg_queue);

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
	//int retVal = k_send_message( KB_I_PROCESS_ID, message_envelope);



	kbd_i_proc(0);
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
		sigaddset(&newmask, SIGUSR1); // the CRT signal
		sigaddset(&newmask, SIGUSR2); // the KB signal
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
