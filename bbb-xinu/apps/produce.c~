#include <xinu.h>
#include <prodcons.h>

void producer(const int count){
      //Code to produce values less than equal to count, 
      //produced value should get assigned to global variable 'n'.
      //print produced value e.g. produced : 8
	int temp, i;
	for (i = 1 ; i <= 20; i++){
		temp = n;
		temp += 1;
		n = temp;
		printf("produced: %d\n", n);
		signal(consumed);
		wait(produced);
	}
}

