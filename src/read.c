#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "read.h"

void CURRENT_READ_VALUE(int integerDataId){

FILE *fp;
int integerValue = (int)integerDataId;
char *dataId = malloc(8*sizeof(char));
snprintf(dataId, 8, "%d", integerValue);
fp = fopen(dataId,"r");
int failure = 0;
if(fp == NULL){
	failure = 1;
	printf("Error in reading the data from file %s\n",dataId);
}
else{
  failure = 1;
		//Check if the data_Id has been committed or not to read from the cell storage.
  char secondLine[50] = "";
  strcat(secondLine,"ACTION_ID = ");
  strcat(secondLine, dataId);
  strcat(secondLine,"\n");
  FILE* f;
  long sz;
  f = fopen("log.txt", "rb");
  sz = fsize(f);
  if (sz > 0)
  {
    char buf[256];
    fseek(f, sz, SEEK_SET);
    char prevScannedLine[256];
    //The below commented line if included will not read the value if that particular value is not committed or aborted. 
    //This can be handled using locks
    //while (fgetsr(buf, sizeof(buf), f) != NULL){
      //check if previous line is type = outcome
      //	if(strcmp(buf,"Type = Outcome\n") == 0 && strcmp(prevScannedLine,secondLine) == 0){      		
      		//changing failure to zero since the transaction has been committed or aborted. 
          //If its in pending state need to throw an error to user
    		failure = 0;		
      //  break;
      //	}
      //  strcpy(prevScannedLine,buf);
    //}
  }
  fclose(f);
	}

	if(failure == 0){
			char value[256];
			while (fscanf(fp, "%s", &value) != EOF) {
			  printf("%s \n", value);
			}
	}
  else{
    //Failure is one
    printf("Please commit or abort transaction to read the value\n");

  }
  fclose(fp);
}