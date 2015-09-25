#include <xinu.h>
#include <stdlib.h> 
#include <stdio.h>  
#include <stddef.h>

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

/*function Prototype*/
void consumer(const int count);
void producer(const int count);

void consumer(const int count)
{
     //Code to consume values of global variable 'n' until the value
     //of n is less than or equal to count
     //print consumed value e.g. consumed : 8
	int tmp;
	while(1){
	tmp = n;
	if (tmp <= count && tmp > 0){
		tmp--;
		printf("consumed: %d\n", tmp);
		n = tmp;
	}
      	else
		sleep(1);
	}
 }

 void producer(const int count)
 {
      //Code to produce values less than equal to count, 
      //produced value should get assigned to global variable 'n'.
      //print produced value e.g. produced : 8
	int tmp;
	while(1){

	tmp = n;
	if (tmp > count)
		sleep(1);
	else{
		tmp++;
      		printf("produced: %d\n", tmp);
		n = tmp;
	}
	}
 }
