#include <xinu.h>
#include "future.h"

syscall future_free(struct future* f){
	if (NULL != f){
		freemem((char*)f, sizeof(struct future));
		return OK;
	}
	return SYSERR;
}
