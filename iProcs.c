#include "iProcs.h"
#include "rtx.h"
#include "kbcrt.h"
#include "MsgEnvQueue.h"
#include "kernal.h"

void crt_i_proc(int signum)
{
	int error = k_pseudo_process_switch(CRT_I_PROCESS_ID);

	if (error != SUCCESS)
	{
		printf("Error! Process Switch failed in CRT I process");
		cleanup();
		return;
	}

	if (signum == SIGUSR2)
	{
		MsgEnv* envTemp = NULL;
		envTemp = MsgEnvQ_dequeue(displayQ);
		if (envTemp == NULL)
		{
			printf("Warning: Recieved a signal in CRT I process but there was no message.");
			return;
		}
		envTemp->msg_type = DISPLAY_ACK;
		k_send_message(P_PROCESS_ID, envTemp);
		k_return_from_switch();
		return;
	}

	MsgEnv* env = k_receive_message();
	outputbuf command;

	if (env==NULL) {
		env = k_receive_message();
	}
	strcpy(in_mem_p_crt->outdata,env->data);

	MsgEnvQ_enqueue(displayQ,env);
	in_mem_p_crt->ok_flag =  OKAY_DISPLAY;

	k_return_from_switch();
	return;
}

void kbd_i_proc(int signum)
{
	int error = k_pseudo_process_switch(KB_I_PROCESS_ID);
	if (error != SUCCESS)
	{
		printf("Error! Context Switch failed in keyboard I process");
		cleanup();
	}

	MsgEnv* env = k_receive_message();

	if (env != NULL)
	{
		// Loop until writing in shared memory is done
		while (in_mem_p_key->ok_flag==OKAY_TO_WRITE);

		memcpy(env->data,in_mem_p_key->indata,in_mem_p_key->length + 1);

		// Send message back to process that called us
		k_send_message(env->sender_pid ,env);

		in_mem_p_key->ok_flag = OKAY_TO_WRITE; // okay to write again
		k_return_from_switch();
		return;
	}
	k_return_from_switch();
	return;
}
