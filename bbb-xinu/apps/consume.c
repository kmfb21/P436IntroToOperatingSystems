#include <prodcons.h>

void consumer(int count)
{
     //Code to consume values of global variable 'n' until the value
     //of n is less than or equal to count
     //print consumed value e.g. consumed : 8
	while(true){

	if (n <= count && n > 0){
		n--;
		printf("consumed: %d\n", n);
	}
      	else
		sleep(1);
	}
 }
