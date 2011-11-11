#include "iProcs.h"
#include "rtx.h"
#include "kbcrt.h"

void crt_i_proc(int signum)
{
	int error = k_pseudo_process_switch(CRT_I_PROCESS_ID);
	if (error != SUCCESS)
		ps("CRT I PROC ERROR!");

	ps("Inside CRT I proc");


	if (signum == SIGUSR2) {
		if (DEBUG==1) {
			fflush(stdout);
			printf("Current PCB msgQ size is %i for process 1\n", MsgEnvQ_size(current_process->rcv_msg_queue) );
			ps("Got SIGUSR2");
		}
			MsgEnv* envTemp = NULL;
			envTemp = (MsgEnv*)MsgEnvQ_dequeue(displayQ);
			envTemp->msg_type = DISPLAY_ACK;
			k_send_message(P_PROCESS_ID,envTemp);
			ps("Display ACK sent by crt");
			k_return_from_switch();
			return;

	}

	MsgEnv* env = (MsgEnv*)k_receive_message();

	outputbuf command;

	while(env==NULL) {
		env = (MsgEnv*)k_receive_message();
	}

	if (DEBUG==1) {
		fflush(stdout);
		printf("Message received by crt i proc\n");
		fflush(stdout);
		printf("Current PCB msgQ size is %i for process 1\n", MsgEnvQ_size(current_process->rcv_msg_queue) );
		printf("The message data section holds \"%s\" \n",env->data);
		fflush(stdout);
	}

	//in_mem_p_crt->outdata[0] = env->data;
	strcpy(in_mem_p_crt->outdata,env->data);

	if (DEBUG==1) {
		printf("The message data section holds \"%s\" \n",in_mem_p_crt->outdata);
	}
	MsgEnvQ_enqueue(displayQ,env);
	//displayQueue = env;
	in_mem_p_crt->ok_flag = 1;

	k_return_from_switch();
	return;
}

void kbd_i_proc(int signum)
{
	int error = k_pseudo_process_switch(KB_I_PROCESS_ID);
	if (error != SUCCESS)
		ps("KBD I PROC ERROR!");

	ps("Inside keyboard I proc");
	MsgEnv* env = (MsgEnv*)k_receive_message();

	if (env != NULL) {

		fflush(stdout);
		ps("Envelope recognized by kbd_i_proc");
		fflush(stdout);


		while (in_mem_p_key->ok_flag==0);

		//if (in_mem_p_key->indata[0] != '\0') return;
		//strcpy(env->data,in_mem_p_key->indata);

		//env->data = "some data\0";
		memcpy(env->data,in_mem_p_key->indata,in_mem_p_key->length + 1);

		//k_send_message(env->sender_pid,env);
		k_send_message(2,env);
		if (DEBUG==1) {
			fflush(stdout);
			printf("Keyboard sent message\n");
			fflush(stdout);
		}
		in_mem_p_key->ok_flag = 0;
		k_return_from_switch();
		return;
	}
	k_return_from_switch();
	return;
}
