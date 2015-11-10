#include <xinu.h>
#include <prodcons.h>

int future_cons(struct future *fut) {
//printf("process %d start\n", getpid());
  int i, status;
  status = future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("process %d consumed %d\n", getpid(), i);
  return 0;
}
