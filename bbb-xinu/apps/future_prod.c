#include <xinu.h>
#include <prodcons.h>

int future_prod(struct future *fut, int count) {
  int i, j;
  j = (int)fut;
  for (i=0; i<=count; i++) {
    j += i;
  }
  future_set(fut, &j);
/*
  if (fut->flag == FUTURE_EXCLUSIVE)
     printf("exclusive mode produced %d\n", j);
  else if (fut->flag == FUTURE_SHARED)
  printf("shared mode produced %d\n", j);
  else
  printf("queue mode produced %d\n", j);
*/
  return 0;
}
