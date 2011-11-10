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
	MsgEnv* ret = k_receive_message();
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
int get_console_chars(MsgEnv *message_envelope){
	atomic(TRUE);
	printf("Atomic true works\n");
	int ret = k_get_console_chars(message_envelope);
	printf("Got console chars\n");
	atomic(FALSE);
	return ret;
}

int release_message_env(MsgEnv* env){
	atomic(TRUE);
	int ret = k_release_message_env(env);
	atomic(FALSE);
	return ret;

}
MsgEnv* request_msg_env(){
	atomic(TRUE);
	int ret = k_request_msg_env();
	atomic(FALSE);
	return ret;
}

