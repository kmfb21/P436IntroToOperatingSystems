#include <xinu.h>
#include "future.h"

struct future* future_alloc(int future_flags){
	struct future* f = (struct future*)getmem(sizeof(struct future));
	if (NULL == f)
		return f;
	f->flag = future_flags;
	f->state = FUTURE_EMPTY;
	return f;
}
