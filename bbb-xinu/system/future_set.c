#include <xinu.h>
#include "future.h"
syscall future_set(struct future* f, int* value){
	if (NULL == f)
		return SYSERR;

	switch (f->flag){
	case FUTURE_EXCLUSIVE:
		if (FUTURE_EMPTY != f->state)
			return SYSERR;
		f->state = FUTURE_WAITING;	
		f->value = value;
		f->state = FUTURE_VALID;	
		if (0 != f->pid)
			//resume the process trying to get the future
			resume(f->pid);
		break;
	case  FUTURE_SHARED:
//only one set is allowed for future
		if (FUTURE_EMPTY != f->state)
			return SYSERR;
		f->state = FUTURE_WAITING;	
		f->value = value;
		f->state = FUTURE_VALID;
		//resume all get processes within the get_queue in the future, and pop each get process out of the get queue.
		struct queue* tmp = f->get_queue;
		while (tmp){
			f->get_queue = f->get_queue->next;
			//printf("resume process %d\n", tmp->pid);
			resume(tmp->pid);
			freemem((char*)tmp, sizeof(struct queue));
			tmp = f->get_queue;
		}
		break;

	case FUTURE_QUEUE:
		if (NULL != f->get_queue){
			//if get_queue is not empty, meaning set_queue is empty
			//so we only pop one get process from the get_queue and that's it
			f->state = FUTURE_WAITING;	
			f->value = value;
			f->state = FUTURE_VALID;
			struct queue* tmp = f->get_queue;
			f->get_queue = f->get_queue->next;
			resume(tmp->pid);
			//freemem((char*)tmp, sizeof(struct queue));
			
		}
		else{
			//but if get_queue is empty, then we check set_queue is empty or not
			pid32 p = getpid();
			if (NULL == f->set_queue){
				//if set_queue is empty, we add the current set process to the empty set queue
				f->set_queue = (struct queue*)getmem(sizeof(struct queue));
				f->set_queue->next = NULL;
				f->set_queue->pid = p;
			}
			else{
//if set_queue is not empty, we push current set process to the queue tail.
				struct queue* tmp = f->set_queue;
				while (tmp->next){
					tmp = f->set_queue->next;
				}
				tmp->next = (struct queue*)getmem(sizeof(struct queue));
				tmp->next->pid = p;
				tmp->next->next = NULL;
			}
			suspend(p);
			//after resumed, set process pop it out and set future to the original state
			f->state = FUTURE_WAITING;
			f->value = value;
			f->state = FUTURE_VALID;
			if (0 != f->pid)
				resume(f->pid);
		}
		break;
	default:
		break;
	}
	return OK;
	
}
