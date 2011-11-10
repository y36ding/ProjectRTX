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
	int ret = k_get_console_chars(message_envelope);
	atomic(FALSE);
	return ret;
}


