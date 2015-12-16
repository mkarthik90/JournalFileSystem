#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "scanner.h"
#include "aborting.h"

int aborting(int integerDataId){

struct flock lock;
memset (&lock, 0, sizeof(lock));
char *dataId = malloc(8*sizeof(char));
snprintf(dataId, 8, "%d", integerDataId);
char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, dataId);
strcat(secondLine,"\n");

//Check if this data Id is already Committed or not. 

FILE* fp;
  long size;
  int available = 0;
 
fp = fopen("log.txt", "rb");
int fd = fileno(fp);
lock.l_type = F_WRLCK;
fcntl (fd, F_SETLKW, &lock);

size = fsize(fp);

  if (size > 0)
  {
    char buf[256];
    fseek(fp, size, SEEK_SET);
    
    while (fgetsr(buf, sizeof(buf), fp) != NULL){
        if(strcmp(buf,secondLine) == 0){
          available = 1;
          fgetsr(buf, sizeof(buf), fp);
        }
        if(strcmp(buf,"Type = Outcome\n") == 0 && available == 1){
          printf("\nCannot Abort already Completed transaction for %s\n",dataId);
          //Sending Error Code == 1 if transaction is already Committed
          return 1;
        }
        available = 0;
    }
  }
  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  fclose(fp);


  //Continue with Abort

  FILE* f;
  long sz;
  available = 0;
 
  if ((f = fopen("log.txt", "rb")) == NULL)
  {
    //This condition should not happen
  }

  sz = fsize(f);

  if (sz > 0)
  {

    char buf[256];
    fseek(f, sz, SEEK_SET);
    char undoOperationValue[256];
    char undoValue [256];
    char undoVariableName[256];
    char prevScannedLine[256];
    
    while (fgetsr(buf, sizeof(buf), f) != NULL){
        

      	// Logic for undo Operation
      	if(strcmp(buf,"Undo Operation\n") == 0){
      		strcpy(undoOperationValue,prevScannedLine);
      	}

      	if(strcmp(buf,"TYPE = CHANGE\n") == 0 && strcmp(prevScannedLine,secondLine) == 0){

      		int i=0,j=0;
	      	for(i=0; undoOperationValue[i]!='=';i++){
	      		//Iterate till this = is obtained. After that get the value for undo
	      		undoVariableName[i] = undoOperationValue[i];
            undoVariableName[i+1] = '\0';
	      	}
	      	//To Remove Blank Space
	      	i = i+1 ;
	      	for(j=0;undoOperationValue[i]!='\0';j++,i++){
	      		undoValue[j] = undoOperationValue[i];
	      	}
	      	
	      	undoValue[j] = '\0';
      		//Write the value to the location and log the result
          //So in recover procedure. When the outcome is not in log we perform a undo operation

      FILE *fp;
      fp = fopen(undoVariableName,"w");
      fd = fileno(fp);
      lock.l_type = F_WRLCK;
      fcntl (fd, F_SETLKW, &lock);
      fprintf(fp, "%s", undoValue);
      lock.l_type = F_UNLCK;
      fcntl (fd, F_SETLKW, &lock);
      fclose(fp);
			
			FILE *logFP;
			logFP = fopen("log.txt","a");
			fprintf(logFP, "\n%s\n", "Type = Outcome");
			fprintf(logFP, "%s", secondLine);
			fprintf(logFP, "%s\n", "Status = Aborted");
			fclose(logFP);

			
      	}
      	
      	strcpy(prevScannedLine,buf);
    }
    	

  }

  fclose(f);
return 0;
}