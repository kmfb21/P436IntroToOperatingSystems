#include <prodcons.h>

 void producer(const int count)
 {
      //Code to produce values less than equal to count, 
      //produced value should get assigned to global variable 'n'.
      //print produced value e.g. produced : 8
	while(true){
	if (n > count)
		sleep(1);
	else{
		n++;
      		printf("produced: %d\n", n);
	}
	}
 }
