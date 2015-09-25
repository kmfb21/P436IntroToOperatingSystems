#include <prodcons.h>
int n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
      //Argument verifications and validations

      int count = 2000;             //local varible to hold count
      if (nargs == 2){
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
      resume( create(producer, 1024, 20, "producer", 1, count) );
      resume( create(consumer, 1024, 20, "consumer", 1, count) );
}
