#ifndef _MSGENVQ_HEADER_
#define _MSGENVQ_HEADER_

#include "rtx.h"

MsgEnvQ* MsgEnvQ_create();
void MsgEnvQ_destroy(MsgEnvQ *destroyedQ);
int MsgEnvQ_is_empty(MsgEnvQ* emptyQ);
MsgEnv* MsgEnvQ_dequeue(MsgEnvQ *msg_env_q);
int MsgEnvQ_enqueue(MsgEnvQ *msg_env_q, MsgEnv* env);
int MsgEnvQ_size(MsgEnvQ *msg_env_q);

#endif
