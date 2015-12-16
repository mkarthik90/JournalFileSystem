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

/* This is a sample code to create a fault in the application and then call recover method. 
   In this method two transactions remain in pending state and those two transactions will be rolled back by the recover procedure*/
void testMultiThreadFault1(){

	int variableName = WRITE_NEW_VALUE("45");
	commit(variableName);
	int varaibleTwo =  WRITE_NEW_VALUE("3453");
	int varaibleThree =  WRITE_NEW_VALUE("56");
	commit(varaibleThree);
	aborting(varaibleTwo);
	int variableFour = WRITE_NEW_VALUE("3453");

	int transactionId5 = beginTransaction();
	writeExistingValue("testingWithOldValue",2,transactionId5);
	commit(transactionId5);

	int transactionId6 = beginTransaction();
	writeExistingValue("TestingWithNewValue",2,transactionId6);
		
	//TransactionId 6 and variableFour needs to be recovered. 
	recover();

}

