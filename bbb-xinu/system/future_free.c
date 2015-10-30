#include <xinu.h>
#include "future.h"

syscall future_free(struct future* f){
	if (NULL != f){
		if (NULL != f->set_queue){
			struct queue* tmp;
			while (f->set_queue){
				tmp = f->set_queue->next;
				freemem((char*)f->set_queue, sizeof(struct queue));
				f->set_queue = tmp;
			}
		}
		if (NULL != f->get_queue){
			struct queue* tmp;
			while (f->get_queue){
				tmp = f->get_queue->next;
				freemem((char*)f->get_queue, sizeof(struct queue));
				f->get_queue = tmp;
			}
		}
		
		freemem((char*)f, sizeof(struct future));
		return OK;
	}
	return SYSERR;
}
