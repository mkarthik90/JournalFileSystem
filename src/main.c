#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "read.h"
#include "write.h"
#include "commit.h"
#include "aborting.h"
#include "transaction.h"
#include "write1.h"
#include "recover.h"

void main(int argc, char *argv[]){

	if(strcmp("WRITE_NEW_VALUE",argv[1]) == 0){
		int variableName = WRITE_NEW_VALUE(argv[2]);
		printf("Variable Name and Data Id is %d\n",variableName);
	}

	else if(strcmp("COMMIT",argv[1]) == 0){
		int transactionId = atoi(argv[2]);
		commit(transactionId);
	}
	else if(strcmp("READ_CURRENT_VALUE",argv[1]) == 0){
		int variableName = atoi(argv[2]);
		CURRENT_READ_VALUE(variableName);
	}
	else if(strcmp("ABORT",argv[1]) == 0){
		int transactionId = atoi(argv[2]);
		aborting(transactionId);
	}
	else if(strcmp("BeginTransaction",argv[1]) == 0){
		int transactionId = beginTransaction();
		printf("Being Transaction %d\n",transactionId);
	}
	else if(strcmp("write",argv[1]) == 0){
		//argv[2] = Value to be written
		//argv[3] = Existing variable to be overwritten
		//argv[4]  = Transaction Id
		int variableName = atoi(argv[3]);
		int transactionId = atoi(argv[4]);
		writeExistingValue(argv[2],variableName,transactionId);
	}
	else if(strcmp("Recover",argv[1]) == 0){
		recover();
	}
	else{
		printf("Command Not avaialable\n");
	}
}