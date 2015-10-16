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
	if ((nargs == 3 && strncmp(args[2], "-f", 2) == 0) ||
	(nargs == 2 && strncmp(args[1], "-f", 2) == 0)){
		use_future = 1;
	}

      if ((nargs == 2 && strncmp(args[1], "-f", 2) != 0) || nargs == 3){
      //check args[1] if present assign value to count
      count = atoi(args[1]);
      if (count <= 0){
	printf("Count must be a positive integer", count);
      	return 0;
	}
     }

      //create the process producer and consumer and put them in ready queue.
      //Look at the definations of function create and resume in exinu/system folder for reference.    
	n = 0;  
	if (!use_future){
      		resume( create(producer, 1024, 20, "producer", 1, count) );
      		resume( create(consumer, 1024, 20, "consumer", 1, count) );
	}
	else{
		struct future *f1, *f2, *f3;
 
  f1 = future_alloc(FUTURE_EXCLUSIVE);
  f2 = future_alloc(FUTURE_EXCLUSIVE);
  f3 = future_alloc(FUTURE_EXCLUSIVE);
 
  resume( create(future_cons, 1024, 20, "fcons1", 1, f1) );
  resume( create(future_prod, 1024, 20, "fprod1", 1, f1) );
  resume( create(future_cons, 1024, 20, "fcons2", 1, f2) );
  resume( create(future_prod, 1024, 20, "fprod2", 1, f2) );
  resume( create(future_cons, 1024, 20, "fcons3", 1, f3) );
  resume( create(future_prod, 1024, 20, "fprod3", 1, f3) );
	}
}
