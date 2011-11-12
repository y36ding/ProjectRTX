#include <stdio.h>
#include <signal.h>
#include "MsgEnvQueue.h"
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/wait.h>


MsgEnvQ* MsgEnvQ_create()
{
	MsgEnvQ* new_MsgEnvQ = malloc(sizeof(MsgEnvQ));
	if (new_MsgEnvQ )
	{
		new_MsgEnvQ->head = NULL;
		new_MsgEnvQ->tail = NULL;
	}
	return new_MsgEnvQ;
}

void MsgEnvQ_destroy(MsgEnvQ *destroyedQ)
{
	free(destroyedQ);
}

int MsgEnvQ_is_empty(MsgEnvQ* emptyQ)
{
	int empty = 1;
	if (emptyQ == NULL)
		return NULL_ARGUMENT;
	

	if (emptyQ->head == NULL)
	{
		emptyQ->tail=NULL;
	}else{
		empty = 0;
	}

	return empty;
}

MsgEnv *MsgEnvQ_dequeue(MsgEnvQ *msg_env_q)
{
	if (msg_env_q == NULL)
		return NULL;

	MsgEnv* dequeued_env = msg_env_q->head; 
	if (msg_env_q->head == msg_env_q->tail)
	{
		msg_env_q->head = NULL;
		msg_env_q->tail = NULL;
	}

	if (msg_env_q->head != NULL)
	{
		msg_env_q->head = msg_env_q->head->next;     		
	}

	dequeued_env->next = NULL;
	return dequeued_env;
}

int MsgEnvQ_enqueue(MsgEnvQ *msg_env_q, MsgEnv* env)
{
	if (env == NULL || msg_env_q == NULL)
	{
		return NULL_ARGUMENT;
	}

	if (msg_env_q->head != NULL)
	{
		msg_env_q->tail->next = env;
	}else{
		msg_env_q->head = env;		
	}

	msg_env_q->tail = env;
	msg_env_q->tail->next = NULL;

	return 0;
}

int MsgEnvQ_size(MsgEnvQ *msg_env_q)
{
	if (msg_env_q == NULL)
	{
		return NULL_ARGUMENT;
	}

	MsgEnv *env = msg_env_q->head;
	int length = 0;

	while (env != NULL)
	{
		env = env->next;
		length++;
	}

	return length;
}
