#include <xinu.h>
#include <prodcons.h>

void consumer(const int count){
     //Code to consume values of global variable 'n' until the value
     //of n is less than or equal to count
     //print consumed value e.g. consumed : 8
	int temp, i;
	wait(consumed);
	for (i = 1; i <= 20; i++){
		printf("consumed: %d\n", n);
		signal(produced);
		wait(consumed);			
	}
}

