#include <kernel.h>
#include <future.h>

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(const int);
void producer(const int);
int future_cons(struct future *);
int future_prod(struct future *, int);

/* Semaphore Declarations */
extern sid32 produced, consumed;



