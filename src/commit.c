#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "scanner.h"
#include "commit.h"


int commit(int integerDataId){

char *dataId = malloc(8*sizeof(char));
snprintf(dataId, 8, "%d", integerDataId);

char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, dataId);
strcat(secondLine,"\n");

//Check if this data Id is already Committed or not. 

struct flock lock;
memset (&lock, 0, sizeof(lock));
FILE* f;
  long sz;
  int available = 0;
 
f = fopen("log.txt", "rb");
int fd = fileno(f);
lock.l_type = F_RDLCK;
fcntl (fd, F_SETLKW, &lock);
sz = fsize(f);

  if (sz > 0)
  {
    char buf[256];
    fseek(f, sz, SEEK_SET);
    
    while (fgetsr(buf, sizeof(buf), f) != NULL){
      	if(strcmp(buf,secondLine) == 0){
      		available = 1;
          fgetsr(buf, sizeof(buf), f);
      	}
      	if(strcmp(buf,"Type = Outcome\n") == 0 && available == 1){
      		printf("\nCannot Commit already Completed transaction for %s\n",dataId);
      		//Sending Error Code == 1 if transaction is already Committed
      		return 1;
      	}
        available = 0;
    }
  }
  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  fclose(f);

//Log the change in Log file only if transaction has not been completed by checking completed ==0
 
FILE *logFP;
logFP = fopen("log.txt","a");
fd = fileno(f);
lock.l_type = F_WRLCK;
fcntl (fd, F_SETLKW, &lock);
fprintf(logFP, "\n%s", "Type = Outcome");
fprintf(logFP, "\n%s", secondLine);
//Already \n appended to secondLine
fprintf(logFP, "%s\n", "Status = Committed");
lock.l_type = F_UNLCK;
fcntl (fd, F_SETLKW, &lock);
fclose(logFP);
return 0;
}
