#include "userAPI.h"
#include "kernal.h"

int send_message(int dest_process_id, MsgEnv *msg_envelope)
{
	atomic(TRUE);
	int ret = k_send_message(dest_process_id, msg_envelope);
	atomic(FALSE);
	return ret;
}
MsgEnv *receive_message()
{
	atomic(TRUE);
	if (DEBUG==1) {
		fflush(stdout);
		printf("Calling receive message\n");
		fflush(stdout);
	}
	MsgEnv* ret = k_receive_message();
	if (DEBUG==1) {
		fflush(stdout);
		printf("Receive message returned\n");
		fflush(stdout);
	}
	atomic(FALSE);
	return ret;
}
int send_console_chars(MsgEnv *message_envelope)
{
	atomic(TRUE);
	int ret = k_send_console_chars(message_envelope);
	atomic(FALSE);
	return ret;
}
int get_console_chars(MsgEnv *message_envelope)
{
	atomic(TRUE);
	int ret = k_get_console_chars(message_envelope);
	if (DEBUG==1) {
		printf("Current PCB msgQ size is %i for process 0\n", MsgEnvQ_size(pid_to_pcb(KB_I_PROCESS_ID)->rcv_msg_queue) );
		//printf("Current PCB msgQ size is %i for process 2\n", MsgEnvQ_size(pid_to_pcb(P_PROCESS_ID)->rcv_msg_queue) );
	}
	atomic(FALSE);
	return ret;
}

int release_message_env(MsgEnv* env){
	atomic(TRUE);
	MsgEnv* ret = (MsgEnv*)k_release_message_env(env);
	atomic(FALSE);
	return ret;
}

MsgEnv* request_msg_env(){
	atomic(TRUE);
	MsgEnv* ret = k_request_msg_env();
	atomic(FALSE);
	return ret;
}

