#include<stdio.h>
#include<string.h>
#include<memory.h>
#include<stdlib.h>
#include<unistd.h>

int getUniqueDataId = 0;




/* File must be open with 'b' in the mode parameter to fopen() */
long fsize(FILE* binaryStream)
{
  long ofs, ofs2;
  int result;

  if (fseek(binaryStream, 0, SEEK_SET) != 0 ||
      fgetc(binaryStream) == EOF)
    return 0;

  ofs = 1;

  while ((result = fseek(binaryStream, ofs, SEEK_SET)) == 0 &&
         (result = (fgetc(binaryStream) == EOF)) == 0 &&
         ofs <= LONG_MAX / 4 + 1)
    ofs *= 2;

  /* If the last seek failed, back up to the last successfully seekable offset */
  if (result != 0)
    ofs /= 2;

  for (ofs2 = ofs / 2; ofs2 != 0; ofs2 /= 2)
    if (fseek(binaryStream, ofs + ofs2, SEEK_SET) == 0 &&
        fgetc(binaryStream) != EOF)
      ofs += ofs2;

  /* Return -1 for files longer than LONG_MAX */
  if (ofs == LONG_MAX)
    return -1;

  return ofs + 1;
}

/* File must be open with 'b' in the mode parameter to fopen() */
/* Set file position to size of file before reading last line of file */
char* fgetsr(char* buf, int n, FILE* binaryStream)
{
  long fpos;
  int cpos;
  int first = 1;

  if (n <= 1 || (fpos = ftell(binaryStream)) == -1 || fpos == 0)
    return NULL;

  cpos = n - 1;
  buf[cpos] = '\0';

  for (;;)
  {
    int c;

    if (fseek(binaryStream, --fpos, SEEK_SET) != 0 ||
        (c = fgetc(binaryStream)) == EOF)
      return NULL;

    if (c == '\n' && first == 0) /* accept at most one '\n' */
      break;
    first = 0;

    if (c != '\r') /* ignore DOS/Windows '\r' */
    {
      unsigned char ch = c;
      if (cpos == 0)
      {
        memmove(buf + 1, buf, n - 2);
        ++cpos;
      }
      memcpy(buf + --cpos, &ch, 1);
    }

    if (fpos == 0)
    {
      fseek(binaryStream, 0, SEEK_SET);
      break;
    }
  }

  memmove(buf, buf + cpos, n - cpos);

  return buf;
}


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



int commit(char *dataId){


char secondLine[50] = "";
strcat(secondLine,"ACTION_ID = ");
strcat(secondLine, dataId);

       


//Check if this data Id is already Committed or not. 



FILE* f;
long sz = fsize(f);

  if (sz > 0)
  {
    char buf[256];
    fseek(f, sz, SEEK_SET);
    while (fgetsr(buf, sizeof(buf), f) != NULL)
      printf("%s", buf);
  }

  fclose(f);



//Log the change in Log file
FILE *logFP;
logFP = fopen("log.txt","a");
fprintf(logFP, "\n%s\n", "Outcome = Commit");
fprintf(logFP, "%s\n", secondLine);
fclose(logFP);

return 0;


}


void main(){

	remove("log.txt");

	char *dataId1 = WRITE_NEW_VALUE(1234);

	
	/*printf("Data Id 1  ");
	printf("%s\n",dataId1);*/

	char *dataId2 = WRITE_NEW_VALUE(5678);
	
	//String copy not working
	/*char secondFile[10];
	strcpy(secondFile,dataId2);
	printf("Second File %s\n",secondFile);*/


	char *dataId3 = WRITE_NEW_VALUE(9101);


	commit(dataId1);
	
	CURRENT_READ_VALUE(dataId1);
	CURRENT_READ_VALUE(dataId2);
	CURRENT_READ_VALUE(dataId3);



	//CURRENT_READ_VALUE(dataId1);
	//CURRENT_READ_VALUE(dataId2);
	
}


