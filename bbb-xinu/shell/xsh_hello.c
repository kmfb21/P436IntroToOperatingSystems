/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - obtain and print welcome string
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[]) {	
	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s <string>\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays the welcome string\n");
		printf("Options (one per invocation):\n");
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	printf("Hello %s, Welcome to the world of Xinu\n", args[1]);
	return 0;
}
