#include <xinu.h>
#include "future.h"
syscall future_set(struct future* f, int* value){
	if (NULL == f)
		return SYSERR;
	if (FUTURE_EMPTY != f->state)
		return SYSERR;
	//TODO assignment 5 may need to change
	if (FUTURE_EXCLUSIVE != f->flag)
		return SYSERR;
	f->state = FUTURE_WAITING;	
	f->value = value;
	f->state = FUTURE_VALID;
	return OK;
}
