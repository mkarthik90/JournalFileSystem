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
void testFault(){
	remove("log.txt");
	remove("transaction.txt");
	remove("1");
	remove("2");
	remove("3");
	remove("4");

	WRITE_NEW_VALUE("45");
	commit(1);
	WRITE_NEW_VALUE("3453");
	WRITE_NEW_VALUE("56");
	commit(3);
	aborting(2);
	
	
	
	WRITE_NEW_VALUE("3453");

	int transactionId5 = beginTransaction();
	writeExistingValue("testingWithOldValue",2,transactionId5);
	commit(transactionId5);

	int transactionId6 = beginTransaction();
	writeExistingValue("TestingWithNewValue",2,transactionId6);
		

	//Transaction 6 needs to be rolled back since its in pending state and set the value testingWithOldValue
	//Transaction 4 needs to be rolled back and set to default value
	recover();

}

