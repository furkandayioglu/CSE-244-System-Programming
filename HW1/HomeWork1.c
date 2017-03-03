#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int main(int argc, char**argv) {

	FILE* listFromFileLog;
	FILE* listFile;

	int status = 0;
	int line = 1, column = 0, startIndex = 0, endIndex = 0, lenght = 0, count =
			0, i, j;
	char * string = argv[1];
	char* buff; /* buffer used to push the letter to check  both strings are equal */
	char temp = '\0';

	if (argc < 3) {
		perror("!!Usage!!");
		perror("Not enough command Line argument for program \n");
		perror(
				"argv[0] : program name \nargv[1]:string to search \nargv[2]: filename \n");
		return -1;

	}

	buff = (char*) calloc(strlen(string) + 1 , sizeof(char));
	listFromFileLog = fopen("ListFromFile.log", "w");
	listFile = fopen(argv[2], "r");

	if (listFile == NULL) {

		perror("File could not ve opened\n");
		_exit(1);
	}

	//status = fscanf(listFile, "%c", &temp);
	i = 1;
	j = 0;

	do {
		status = fscanf(listFile, "%c", &temp);
		++column;
		++startIndex;
		if (temp == '\n') {
			line++;
			column = 0;
		} else if (temp == string[0]) {
			buff[0] = temp;
			int len = strlen(string);
			int diff = len - 1; /* where the lastletter found */
			endIndex = startIndex;
			for (; diff > 0;) {
				status = fscanf(listFile, "%c", &temp);
				if (temp != ' ' && temp != '\n' && temp != '\t') {
					buff[len - diff] = temp;
					--diff;
					/*fprintf(stderr, "%s\n", buff);*/
				}
				++column;
				endIndex++;
			}

			fseek(listFile, -(int) (endIndex - startIndex), SEEK_CUR); /* to find sequential words */

			if (strcmp(string, buff) == 0) {
				count++;

				column -= (endIndex - startIndex);
				/*fprintf(stderr, "<file : %s [%d,%d] string:%s > \n", argv[2],line,
						column, string);*/
				fprintf(listFromFileLog, "<file : %s [%d,%d] string:%s > \n", argv[2],line,
						column, string);
				endIndex = startIndex;
			}


			for (j = strlen(buff) - 1; j >= 0; --j)
				buff[j] = '\0';
		}
	} while (status != EOF);

	fprintf(listFromFileLog, "------------------------------------------------------------\n file: %s string :%s  found :%d", argv[2], string, count);
	free(buff);
	fclose(listFile);
	fclose(listFromFileLog);
	return 0;
}
