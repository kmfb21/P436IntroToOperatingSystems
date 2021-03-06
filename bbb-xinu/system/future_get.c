#include <xinu.h>
#include <future.h>

syscall future_get(struct future* f, int* value){
	if (NULL == f)
		return SYSERR;	
	switch (f->flag){
	case FUTURE_EXCLUSIVE:
//if second get process tries to get value, that's not gonna happen
		if (0 != f->pid)
			return SYSERR;
//mark future as its own, so no second get process can access
		f->pid = getpid();
		if (FUTURE_VALID != f->state){
//suspend iff when future value is not valid
			suspend(f->pid);
		}
		*value = *(f->value);
		//f->state = FUTURE_EMPTY;
		break;
	case  FUTURE_SHARED:
//if future is not valid, meaning future is not ready yet.
		if (FUTURE_VALID != f->state){
			pid32 p = getpid();
//get queue is empty
			if (NULL == f->get_queue){
				f->get_queue = (struct queue*)getmem(sizeof(struct queue));
				f->get_queue->next = NULL;
				f->get_queue->pid = p;
			}
			else{
//get queue is not empty, push itself to the end of the queue
				struct queue* tmp = f->get_queue;
				while (tmp->next){
					tmp = tmp->next;
				}
				tmp->next = (struct queue*)getmem(sizeof(struct queue));
				tmp->next->pid = p;
				tmp->next->next = NULL;
			}
//put itself to hibernation
			suspend(p);
		}
		*value = *(f->value);		
		break;
	case FUTURE_QUEUE:
		if (NULL != f->set_queue){
			//if set_queue is not empty, meaning get_queue is empty
			//so we only pop one set process from the set_queue and that's it
			struct queue* tmp = f->set_queue;
			f->set_queue = f->set_queue->next;
//let us resume the set process
			resume(tmp->pid);
			freemem((char*)tmp, sizeof(struct queue));
			tmp = NULL;

//then suspend itself to wait set process to set the value
			//f->pid = getpid();
			//suspend(f->pid);

			//after resumed by current set process, read the value and empty value
			*value = *(f->value);
			f->state = FUTURE_EMPTY;
			f->pid = 0;	
		}
		else{

			//but if set_queue is empty, then we check get_queue is empty or not
			pid32 p = getpid();
			//printf("push %d\n", p);
			if (NULL == f->get_queue){
//if get queue is empty, we push the current get process to the head of the get queue
				f->get_queue = (struct queue*)getmem(sizeof(struct queue));
				f->get_queue->next = NULL;
				f->get_queue->pid = p;
			}
			else{
//if get queue is not empty, we push the current get process to the tail of the get queue.
				struct queue* tmp = f->get_queue;
				while (tmp->next){
					tmp = tmp->next;
				}
				tmp->next = (struct queue*)getmem(sizeof(struct queue));
				tmp->next->pid = p;
				tmp->next->next = NULL;
			}
			suspend(p);
			//after resumed, get process pop it out and read the value
			*value = *(f->value);
			struct queue* tmp = f->get_queue;
			struct queue* prev = tmp;
			while (tmp){
				prev = tmp;
				if (p == tmp->pid)
					break;
				tmp = tmp->next;
			}
			prev = tmp->next;
			if (NULL != tmp)
				freemem((char*)tmp, sizeof(struct queue));
			f->state = FUTURE_EMPTY;
		}		
		break;
	default:
		return SYSERR;
	}
	return OK;
}
