#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include "write.h"
#include "transaction.h"


int WRITE_NEW_VALUE(char *value){

int getUniqueDataId  = beginTransaction();
char *fileName = malloc(8*sizeof(char));
snprintf(fileName, 8, "%d", getUniqueDataId);
char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, fileName);

/* Initialize the flock structure. */
struct flock lock;
memset (&lock, 0, sizeof(lock));

//To Check if its an existing file. If so get the value for undo Operation
char oldValue[256]="0"; // If the file does not exist then 0 is written for undo operation
if(access( fileName, F_OK ) != -1 ) {
    //If file exists read the value from file and store for undo operation
    FILE *fp1;
    fp1 = fopen(fileName,"r");	
    int fd1 = fileno(fp1);
    lock.l_type = F_WRLCK;
    fcntl (fd1, F_SETLKW, &lock);
	fscanf (fp1, "%s", &oldValue);	
    lock.l_type = F_UNLCK;
    fcntl (fd1, F_SETLKW, &lock);
    fclose(fp1);

} 

//Log the change in Log file
//Creating value for redo and undo
char *equalTo = "=";
int len = strlen(fileName) + strlen(equalTo) +strlen(value);
    char *redoAction = (char*)malloc(len * sizeof(char) + 1);
    *redoAction = '\0';
    strcat(redoAction,fileName);
    strcat(redoAction, equalTo);
    strcat(redoAction ,value);

	len = strlen(fileName) + strlen(equalTo);
	char *zero = malloc(8*sizeof(char));
	snprintf(zero, 8, "%d", 0);
    len++;//Adding for appending 0
    char *undoAction = (char*)malloc(len * sizeof(char) + 1);
    *undoAction = '\0';
    strcat(undoAction,fileName);
    strcat(undoAction, equalTo);
    strcat(undoAction ,zero);




FILE *logFP;
logFP = fopen("log.txt","a");
int fd = fileno(logFP);
lock.l_type = F_WRLCK;
fcntl (fd, F_SETLKW, &lock);
fprintf(logFP, "\n%s\n", "TYPE = CHANGE");
fprintf(logFP, "%s\n", secondLine);
fprintf(logFP, "%s\n", "Redo Operation");
fprintf(logFP, "%s\n", redoAction);
fprintf(logFP, "%s\n", "Undo Operation");
//If the value is not overwritten we are storing zero as a default value.4
//Action will be same since variable is created for first time
fprintf(logFP, "%s\n", undoAction);
lock.l_type = F_UNLCK;
fcntl (fd, F_SETLKW, &lock);
fclose(logFP);

FILE *fp;
fp = fopen(fileName,"w");
fd = fileno(fp);
lock.l_type = F_WRLCK;
fprintf(fp, "%s", value);
lock.l_type = F_UNLCK;
fcntl (fd, F_SETLKW, &lock);
fclose(fp);
//Converting fileName to integer
int integerFileName = (int)malloc(sizeof(int));
integerFileName = atoi(fileName);
return integerFileName;
}