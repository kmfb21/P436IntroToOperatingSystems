#include <xinu.h>
#include <prodcons.h>

void consumer(const int count){
     //Code to consume values of global variable 'n' until the value
     //of n is less than or equal to count
     //print consumed value e.g. consumed : 8
	int temp;
	wait(consumed);
	while (1){

	if (n == 0){
		signal(produced);
		wait(consumed);
	}
	else{
		temp = n;
		temp -= 1;
		n = temp;
		printf("consumed: %d\n", n);
	}
	
	
	}
}
