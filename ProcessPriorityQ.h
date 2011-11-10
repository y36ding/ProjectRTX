#ifndef _PPQ_HEADER_
#define _PPQ_HEADER_


#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

#include "rtx.h"

#include <sys/mman.h>
#include <sys/wait.h>


typedef struct proc_pq proc_pq_t;

proc_pq_t * proc_pq_create(uint32_t num_priorities);
void proc_pq_destroy(proc_pq_t *ppq);

pcb_t * proc_pq_dequeue(proc_pq_t *ppq);
int proc_pq_enqueue(proc_pq_t *ppq, pcb_t * pcb);

uint32_t proc_pq_get_num_prorities(proc_pq_t *ppq);

pcb_t * proc_pq_remove(proc_pq_t * ppq, pcb_t *pcb);

#endif
