#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "write1.h"
#include "transaction.h"


int writeExistingValue(char *value,int variableName,int transactionId){

char *fileName = malloc(8*sizeof(char));
char *dataId = malloc(8*sizeof(char));
snprintf(fileName, 8, "%d", variableName);
snprintf(dataId, 8, "%d", transactionId);

char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, dataId);

//To Check if its an existing file. If so get the value for undo Operation
char oldValue[256]="0"; // If the file does not exist then 0 is written for undo operation
if( access( fileName, F_OK ) != -1 ) {
    //If file exists read the value from file and store for undo operation
    /* Initialize the flock structure. */
	struct flock lock;
	memset (&lock, 0, sizeof(lock));
    FILE *fp1;
    fp1 = fopen(fileName,"r");	
    int fd = fileno(fp1);
    lock.l_type = F_RDLCK;
    //printf("Waiting for lock for the file %s\n",fileName);
    fcntl (fd, F_SETLKW, &lock);
    
	fscanf (fp1, "%s", &oldValue);	

	lock.l_type = F_UNLCK;
	fcntl (fd, F_SETLKW, &lock);
	fclose(fp1);
	//Log the change in Log file
	//Creating value for redo and undo

	char *equalTo = "=";
	int redoLen = strlen(fileName) + strlen(equalTo) +strlen(value);

	char *redoAction = (char*)malloc(redoLen * sizeof(char) + 1);
    *redoAction = '\0';
    strcat(redoAction,fileName);
    strcat(redoAction, equalTo);
    strcat(redoAction ,value);

    int undoLen = strlen(fileName) + strlen(equalTo) +strlen(oldValue);
	char *undoAction = (char*)malloc(undoLen * sizeof(char) + 1);
    *undoAction = '\0';
    strcat(undoAction,fileName);
    strcat(undoAction, equalTo);
    strcat(undoAction ,oldValue);


	//Including a check to see if the transaction has been already committed. If so send out an erro message.	
	FILE *logFP;
	logFP = fopen("log.txt","a");
	fd = fileno(logFP);
    lock.l_type = F_WRLCK;
    fcntl (fd, F_SETLKW, &lock);

	fprintf(logFP, "\n%s\n", "TYPE = CHANGE");
	fprintf(logFP, "%s\n", secondLine);
	fprintf(logFP, "%s\n", "Redo Operation");
	fprintf(logFP, "%s\n", redoAction);
	fprintf(logFP, "%s\n", "Undo Operation");
	//If the value is not overwritten we are storing zero as a default value.
	fprintf(logFP, "%s\n", undoAction);

	lock.l_type = F_UNLCK;
	fcntl (fd, F_SETLKW, &lock);
	fclose(logFP);

	//Without clicking on enter user breaks the program by giving ctrl+c
	printf("Click Enter to Proceed.\n");
	char ch = getchar();

	FILE *fp;
	fp = fopen(fileName,"w");
	fd = fileno(fp);
	
    lock.l_type = F_WRLCK;
    fcntl (fd, F_SETLKW, &lock);

    //printf("Lock Acquired\n");
	fprintf(fp, "%s", value);

	
	lock.l_type = F_UNLCK;
	fcntl (fd, F_SETLKW, &lock);
	//printf("Lock Released \n");
	fclose(fp);
	//Converting fileName to integer
	int integerFileName = (int)malloc(sizeof(int));
	integerFileName = atoi(fileName);

	return integerFileName;

} 
else{
	//printf("Please create a variable first using WRITE_NEW_VALUE \n");
	//In case user trying to write to a variable that has not been created .
	return 2;
}
}