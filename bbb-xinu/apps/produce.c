#include <xinu.h>
#include <prodcons.h>

void producer(const int count){
      //Code to produce values less than equal to count, 
      //produced value should get assigned to global variable 'n'.
      //print produced value e.g. produced : 8
	int temp;
	while(1){
		if (n == count){
			signal(consumed);
			wait(produced);

		}
		else{
			temp = n;
			temp += 1;
			n = temp;
			printf("produced: %d\n", n);
		}
		
		
	}
}

