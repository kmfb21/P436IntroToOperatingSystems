#include <prodcons.h>
#include <xinu.h>
#include <stdlib.h> 
#include <stdio.h>  
#include <stddef.h>
int n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

sid32 produced, consumed;

shellcmd xsh_prodcons(int nargs, char *args[])
{
    //Argument verifications and validations
  	produced = semcreate(0);
  	consumed = semcreate(1);
	int use_future = 0;
      	int count = 2000;             //local varible to hold count
	if (nargs == 2){
		if (strncmp(args[1], "-f", 2) == 0)
			use_future = 1;
		else{
			count = atoi(args[1]);
		}
	}
	if (nargs == 3 && strncmp(args[1], "-f", 2) == 0){
		use_future = 1;
		count = atoi(args[2]);
	}

	if (count <= 0){
		printf("Count must be a positive integer", count);
      		return 0;
	}

      //create the process producer and consumer and put them in ready queue.
      //Look at the definations of function create and resume in exinu/system folder for reference.    
	n = 0;  
	if (!use_future){
      		resume( create(producer, 1024, 20, "producer", 1, count) );
      		resume( create(consumer, 1024, 20, "consumer", 1, count) );
	}
	else{
		struct future *f_exclusive, *f_shared, *f_queue;
 
  		f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
  		f_shared = future_alloc(FUTURE_SHARED);
  		f_queue = future_alloc(FUTURE_QUEUE);
 
		// Test FUTURE_EXCLUSIVE
  		resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
  		resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, count) );

		// Test FUTURE_SHARED
  		resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
  		resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
  		resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
  		resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
  		resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, count) );

		// Test FUTURE_QUEUE
 		resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
  		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
  		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
  		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
  		resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue, count) );
  		resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue, count) );
  		resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue, count) );
  		resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue, count) );
		
		future_free(f_exclusive);
		future_free(f_shared);
		future_free(f_queue);
	}
	return 0;
}
