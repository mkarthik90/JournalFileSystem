#include<stdio.h>
#include<string.h>
#include<memory.h>
#include<stdlib.h>
#include<unistd.h>

int getUniqueDataId = 0;

void CURRENT_READ_VALUE(char *dataId){


FILE *fp;

fp = fopen(dataId,"r");

	if(fp == NULL){
	printf("Error in reading the data from file %s\n",dataId);
	}
	else{
			int value = 0;
			while (fscanf(fp, "%d", &value) != EOF) {
			  printf("%d \n", value);
			}
	}
}

char *WRITE_NEW_VALUE(int value){

getUniqueDataId ++;

char *fileName = malloc(8*sizeof(char));

snprintf(fileName, 8, "%d", getUniqueDataId);
char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, fileName);

//To Check if its an existing file. If so get the value for undo Operation
int oldValue = 0;
if( access( fileName, F_OK ) != -1 ) {
    //If file exists read the value from file and store for undo operation
    FILE *fp1;
    fp1 = fopen(fileName,"r");	
	fscanf (fp1, "%d", &oldValue);	

} 

//Log the change in Log file
FILE *logFP;
logFP = fopen("log.txt","a");
fprintf(logFP, "\n%s\n", "TYPE = CHANGE");
fprintf(logFP, "%s\n", secondLine);
fprintf(logFP, "%s\n", "Redo Operation");
fprintf(logFP, "%d\n", value);
fprintf(logFP, "%s\n", "Undo Operation");
//If the value is not overwritten we are storing zero as a default value.
fprintf(logFP, "%d\n", oldValue);
fclose(logFP);

FILE *fp;
fp = fopen(fileName,"w");
fprintf(fp, "%d", value);
fclose(fp);
//printf(" Data ID: %s\n",fileName );
return fileName;
}






void main(){

	remove("log.txt");

	char *dataId1 = WRITE_NEW_VALUE(1234);

	
	printf("Data Id 1  ");
	printf("%s\n",dataId1);

	char *dataId2 = WRITE_NEW_VALUE(5678);
	
	//String copy not working
	/*char secondFile[10];
	strcpy(secondFile,dataId2);
	printf("Second File %s\n",secondFile);*/


	char *dataId3 = WRITE_NEW_VALUE(9101);

	CURRENT_READ_VALUE(dataId1);
	CURRENT_READ_VALUE(dataId2);
	CURRENT_READ_VALUE(dataId3);

	//CURRENT_READ_VALUE(dataId1);
	//CURRENT_READ_VALUE(dataId2);
	
}


