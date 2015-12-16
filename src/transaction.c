#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "transaction.h"


int beginTransaction(){

	FILE *fr;
	int transacionId = 0;	
	
	//Using locks to lock the transaction.txt so that next transaction waits until the current transaction completes its execution.

	struct flock lock;

	fr = fopen("transaction.txt","r+");
	int fd = fileno(fr);

	 /* Initialize the flock structure. */
 	memset (&lock, 0, sizeof(lock));
 	lock.l_type = F_WRLCK;

 	/* Place a write lock on the file. */
 	//printf("Waiting to acquire lock on Transaction.txt\n");
 	fcntl (fd, F_SETLKW, &lock);
	//printf("Acquired Lock\n");
	fscanf(fr, "%d", &transacionId);
	
	//This locking is verified by using a getchar statement here. 
	//printf("Press enter to continue\n");
	//char ch = getchar();


	//Uncomment the above two lines to see if the next thread is waiting. The first thread waits in the getchar unless you type a character in keyboard.
	//When you start another call through new terminal . 
	//THe new call waits until this previous call completes. That is user hits some character in keyboard  for getchar


	int newTransactionId = transacionId + 1;
	
	long pos = ftell(fr);
	fseek(fr,0,SEEK_SET);

	fprintf(fr, "%d", newTransactionId);	

	fflush(fr);
	/* Release the lock. */
 	lock.l_type = F_UNLCK;
 	fcntl (fd, F_SETLKW, &lock);
 	//printf("Lock Release on Transaction.txt \n");
	fclose(fr);

	char *fileName = malloc(8*sizeof(char));
	snprintf(fileName, 8, "%d", newTransactionId);
	char secondLine[50] = "";
	strcat(secondLine,"ACTION_ID = ");
	strcat(secondLine, fileName);


	//Locking the log .txt to make sure either it logs the completly or waits for the previous thread to complete its execution
	FILE *logFP;
	logFP = fopen("log.txt","a");
	fd = fileno(logFP);
	lock.l_type = F_WRLCK;
	fcntl (fd, F_SETLKW, &lock);
	fprintf(logFP, "\n%s\n", "TYPE = BEGIN TRANSACTION");
	fprintf(logFP, "%s\n",secondLine);
	lock.l_type = F_UNLCK;
 	fcntl (fd, F_SETLKW, &lock);
	fclose(logFP);

	return newTransactionId;
}

/* Reference: http://www.informit.com/articles/article.aspx?p=23618&seqNum=4 */

