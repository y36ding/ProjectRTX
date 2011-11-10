#ifndef _KERNAL_HEADER_
#define _KERNAL_HEADER_

#include "rtx.h"

void atomic(bool state);

pcb* pid_to_pcb(int dest_process_id);

int k_send_message(int dest_process_id, MsgEnv *msg_envelope);
MsgEnv * k_receive_message();
int k_get_console_chars(MsgEnv *message_envelope);
int k_send_console_chars(MsgEnv *message_envelope);

int k_release_message_env(MsgEnv* env);
MsgEnv* k_request_msg_env();

#endif
