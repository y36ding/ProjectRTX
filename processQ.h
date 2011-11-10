#ifndef _PROCH_HEADER_
#define _PROCH_HEADER_


#include <stdio.h>
#include <signal.h>
#include <fcntl.h>


#include <sys/mman.h>
#include <sys/wait.h>


//typedef struct proc_queue proc_queue_t;

proc_queue_t * proc_queue_create();
void proc_queue_destroy(proc_queue_t * queue);

int proc_queue_is_empty(proc_queue_t * queue);
pcb_t * proc_queue_dequeue(proc_queue_t * queue);
int proc_queue_enqueue(proc_queue_t * queue, pcb_t * pcb);
pcb_t * proc_queue_remove(proc_queue_t * queue, pcb_t *pcb);

#endif
