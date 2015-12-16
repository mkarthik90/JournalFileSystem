#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "scanner.h"
#include "recover.h"
#include "aborting.h"

int recover(){

	//Read from the transaction.txt to get the total number of transactions performed

	FILE *fr;
	int transacionId = 0;	
	if(access( "transaction.txt", F_OK ) != -1){
		fr = fopen("transaction.txt","r");
		fscanf(fr, "%d", &transacionId);
		fclose(fr);
	
	}

	
	//Scan through the log from end and get the list of losers. 
	//Assuming only one transaction can be performed with an actionId. If there are no outcome records then that actionId is a loser.

  int losers[transacionId];
  int completeds[transacionId];
  FILE* f;
  long sz;
  
 
  f = fopen("log.txt", "rb");
  sz = fsize(f);
  int loserIndex = 0;
  if (sz > 0)
  {

    char buf[256];
    fseek(f, sz, SEEK_SET);
    char outcomeRecord[50] = "";
	strcat(outcomeRecord,"Type = Outcome\n");
	int completedsIndex = 0 ;
	char prevScannedLine[256];
	char committed[50] = "";
	strcat(committed,"Status = Committed");

	char aborted[50] = "";
	strcat(aborted,"Status = Aborted");

    while (fgetsr(buf, sizeof(buf), f) != NULL){
		if(strcmp(buf,outcomeRecord) == 0){
			//If the transaction Id has outcome then it is a winner. 
			//There is no concept of end transaction in this implementation
			//Then previous line will contain ACTION_ID = actionIdNumber
			//So
			int k = 11;
			char actionId[256];
			int j =0;
			for(k=11;prevScannedLine[k]!='\n';k++){
				actionId[j] = prevScannedLine[k];
				j++;
			}
			actionId[j] = '\0';
			int completedTransactionId = atoi(actionId);
			completeds[completedsIndex] = completedTransactionId;
			completedsIndex++;
		}		

		if(strcmp(buf,"TYPE = CHANGE\n") == 0){
			// Check if the action Id is completed by logging an outcome record.
			// If it has outcome record it will be in completeds array. else add this actionId to losers list 

			char actionId[256];
			int j=0;
			int k=0;

			//Checking if the actionId is in completeds
			//First get the actionId from previous line
			for(k=11;prevScannedLine[k]!='\n';k++){
				actionId[j] = prevScannedLine[k];
				j++;
			}
			actionId[j] = '\0';
			int transactionIdOfChange = atoi(actionId);
			int transactionCompleted = 0;
			int w =0;
			for(w=0;completeds[w]!=0;w++){
				if(transactionIdOfChange == completeds[w]){
					transactionCompleted = 1;
				}
			}

			if(transactionCompleted == 0){
				losers[loserIndex] = transactionIdOfChange;
				loserIndex++;
			}

			//If transactionIdOfChange means that there is no outcome record so add it to losers.
		}
		strcpy(prevScannedLine,buf);
	}
   }
   fclose(f);
   //Aborting the pending transactions
   int i=0;
   for(i=0;i<loserIndex;i++){
   	aborting(losers[i]);
   }
   return 0;
}