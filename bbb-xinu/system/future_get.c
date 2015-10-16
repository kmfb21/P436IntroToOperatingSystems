#include <xinu.h>
#include <future.h>

syscall future_get(struct future* f, int* value){
	if (NULL == f)
		return SYSERR;
	while (FUTURE_VALID != f->state){
		sleep(1);
		f->pid = getpid();
	}
	*value = *(f->value);
	f->state = FUTURE_EMPTY;
	return OK;
}
