#include <xinu.h>
#include "future.h"

struct future* future_alloc(int future_flags){
	struct future* f = (struct future*)getmem(sizeof(struct future));
	if (NULL == f)
		return NULL;
	f->value = NULL;
	f->flag = future_flags;
	f->state = FUTURE_EMPTY;
	f->pid = 0;
	f->set_queue = NULL;
	f->get_queue = NULL;
	return f;
}
