#include <kernel.h>


/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(const int);
void producer(const int);

/* Semaphore Declarations */
extern sid32 produced, consumed;



