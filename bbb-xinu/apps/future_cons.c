#include <xinu.h>
#include <prodcons.h>

int future_cons(struct future *fut) {

  int i, status;
  status = future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("it produced %d\n", i);
  return 0;
}