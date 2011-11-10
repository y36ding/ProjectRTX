
#include <stdio.h>
#include <signal.h>

#include <fcntl.h>

#include "rtx.h"
#include "ProcessPriorityQ.h"

#include <sys/mman.h>
#include <sys/wait.h>

proc_pq_t * proc_pq_create(uint32_t num_priorities)
{
    if (num_priorities == 0)
    {
        return NULL;
    }

    proc_pq_t * ppq = malloc(sizeof(*ppq));
    if (ppq)
    {
        int allocation_error = 0;
        ppq->priority_queues = malloc(sizeof(*ppq->priority_queues) * num_priorities);
        if (ppq->priority_queues)
        {
            int priority;
            for (priority = 0; priority < num_priorities; priority++)
            {
                ppq->priority_queues[priority] = proc_queue_create();
                allocation_error = allocation_error || !ppq->priority_queues[priority];
            }
        }
        else
        {
            allocation_error = 1;
        }
        ppq->num_priorities = num_priorities;

        if (allocation_error)
        {
            proc_pq_destroy(ppq);
            ppq = NULL;
        }
    }
    return ppq;
}

void proc_pq_destroy(proc_pq_t * ppq)
{
    if (ppq)
    {
        if (ppq->priority_queues)
        {
            int priority;
            for (priority = 0; priority < ppq->num_priorities; priority++)
            {
                 proc_queue_destroy(ppq->priority_queues[priority]);
            }
            free(ppq->priority_queues);
        }
        free(ppq);
    }
}

pcb_t * proc_pq_dequeue(proc_pq_t *ppq)
{
    if (ppq != NULL)
    {
        int priority;
        for (priority = 0; priority < ppq->num_priorities; priority++)
        {
            if (!proc_queue_is_empty(ppq->priority_queues[priority]))
            {
                return proc_queue_dequeue(ppq->priority_queues[priority]);
            }
        }
    }

    return NULL;
}

int proc_pq_enqueue(proc_pq_t *ppq, pcb_t * pcb)
{
    if (ppq == NULL || pcb == NULL)
    {
        //return ERROR_NULL_ARG;
        return 1;
    }

    if (pcb->priority < 0 || pcb->priority >= ppq->num_priorities)
    {
        //return ERROR_ILLEGAL_ARG;
        return 1;
    }

    assert(!pcb->i_process);

    return proc_queue_enqueue(ppq->priority_queues[pcb->priority], pcb);
}

uint32_t proc_pq_get_num_prorities(proc_pq_t *ppq)
{
    return ppq ? ppq->num_priorities : -1;
}

pcb_t * proc_pq_remove(proc_pq_t * ppq, pcb_t *pcb)
{
    if (ppq != NULL && pcb != NULL &&
        pcb->priority >= 0 && pcb->priority < ppq->num_priorities)
    {
        return proc_queue_remove(ppq->priority_queues[pcb->priority], pcb);
    }

    return NULL;
}
