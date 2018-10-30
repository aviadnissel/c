///TESTER FOR EX1
///Author:mattan
///Version 1.0



#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define CHUNKSIZE   4
#define NUMBEROFTESTS 11
int compareFiles(FILE *fp1, FILE *fp2, int filenum)
{
    // fetching character of two file
    // in two variable ch1 and ch2
    char ch1 = (char) getc(fp1);
    char ch2 = (char) getc(fp2);

    // error keeps track of number of errors
    // pos keeps track of position of errors
    // line keeps track of error line
    int error = 0, pos = 0, line = 1;

    // iterate loop till end of file
    while (ch1 != EOF && ch2 != EOF)
    {
        pos++;

        // if both variable encounters new
        // line then line variable is incremented
        // and pos variable is set to 0
        if (ch1 == '\n' && ch2 == '\n')
        {
            line++;
            pos = 0;
        }

        // if fetched data is not equal then
        // error is incremented
        if (ch1 != ch2)
        {
            error++;
	        break;
        }

        // fetching character until end of file
        ch1 = (char) getc(fp1);
        ch2 = (char) getc(fp2);
    }
    if (error==1)
    {
        printf("TEST %d: FAIL\n", filenum);
		printf("WORKING...\n");
    }
    else
    {
	    printf("TEST %d: PASS\n", filenum);
	    printf("WORKING...\n");
    }
	return error;
}


int main(void)
{
	char *filepath;
	char *filename;
	int maxlen = CHUNKSIZE;
	int index = 0;
	int ch;
	int fails = 0;


	// start with a small amount of memory
	filepath = malloc(CHUNKSIZE);                   // small amount of memory
	if (filepath == NULL)                            // check
		exit(1);

	printf("Enter your c executable's full file path with no slash at end:\nfor example: /cs/user/anon/Desktop/ex1\n");

	// read path name char by char
	while ((ch = getchar()) != EOF && ch != '\n')
	{
		filepath[index++] = (char) ch;
		if (index >= maxlen)
		{                      // allow room for a terminator
			maxlen += CHUNKSIZE;                    // more string memory
			filepath = realloc(filepath, (size_t) maxlen);   // get more memory
			if (filepath == NULL)
				exit(1);
		}
	}
	filepath[index] = 0;                            // terminate string input
	printf("You entered: %s\n", filepath);
	char str[80];
	sprintf(str, "cd %s", filepath);
	system(str);
	for (int i = 0; i<NUMBEROFTESTS+1; i++)
	{
		char school[1000];
		sprintf(school, "~labc/www/ex1/AnalyzeProtein %s/tests/test%d >& %s/solutions/school_%d", filepath, i , filepath, i);
		system(school);
		char student[1000];
		sprintf(student, "%s/AnalyzeProtein %s/tests/test%d >& %s/solutions/student_%d", filepath, filepath, i , filepath, i);
		system(student);
		char studentResult[1000];
		char schoolResult[1000];
		sprintf(studentResult, "%s/solutions/student_%d", filepath, i);
		sprintf(schoolResult, "%s/solutions/school_%d", filepath, i);
	    FILE *fp1 = fopen(studentResult, "r");
	    FILE *fp2 = fopen(schoolResult, "r");

	    if (fp1 == NULL || fp2 == NULL)
	    {
	       printf("Error : Bad Directory\n");
	       exit(0);
	    }
	    fails += compareFiles(fp1, fp2, i);
	    // closing both file
	    fclose(fp1);
	    fclose(fp2);
	}






	//USAGE ERROR TEST
	char school[1000];
	sprintf(school, "~labc/www/ex1/AnalyzeProtein >& %s/solutions/school_%d", filepath, NUMBEROFTESTS+1);
	system(school);
	char student[1000];
	sprintf(student, "%s/AnalyzeProtein >& %s/solutions/student_%d",filepath, filepath, NUMBEROFTESTS+1);
	system(student);
	char studentResult[1000];
	char schoolResult[1000];
	sprintf(studentResult, "%s/solutions/student_%d", filepath, NUMBEROFTESTS+1);
	sprintf(schoolResult, "%s/solutions/school_%d", filepath, NUMBEROFTESTS+1);
	FILE *fp1 = fopen(studentResult, "r");
	FILE *fp2 = fopen(schoolResult, "r");

	if (fp1 == NULL || fp2 == NULL)
	{
		printf("Error : Bad Directory\n");
		exit(0);
	}
	fails += compareFiles(fp1, fp2, NUMBEROFTESTS+1);
	// closing both file
	fclose(fp1);
	fclose(fp2);


	if (fails > 0)
		printf("%d more tests to pass. I BELIEVE IN YOU\n", fails);
	else
		printf("Congratulations! You passed all of the tests! Go get something to eat\n");
}

