#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	

typedef struct future{
   int *value;		
   int flag;		
   int state;         	
   pid32 pid;
};

/* Interface for system call */
struct future* future_alloc(int future_flags);
syscall future_free(struct future*);
syscall future_get(struct future*, int*);
syscall future_set(struct future*, int*);
 
#endif /* _FUTURE_H_ */
