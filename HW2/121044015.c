#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>


void listFromFile(char*,char*,FILE*,FILE*);
int openningDirectories(char*,FILE*,FILE*,char*);

	


int main(int argc,char**argv){

int totalCount=0;
int fileCount;
int status;
int i=0;
char* direct;

char filePath[PATH_MAX+1];

FILE* tempFilesName;
FILE* totalWord;
FILE* listFromFileLog;

DIR *mydir;

struct dirent *myfile;
struct stat mystat;

	if(argc<3){
		perror("!!Usage!!");
		perror("Not enough command Line argument for program \n");
		perror("argv[0] : program name \nargv[1]: string \nargv[2]: directory \n");
		_exit(1);

	}

 	tempFilesName=fopen("logPaths.txt","w");
 	totalWord=fopen("wordOfFiles.txt","w");
 	listFromFileLog=fopen("ListFile.log","w");


	direct=argv[1];
    openningDirectories(argv[2],tempFilesName,totalWord,argv[1]);
    
    fclose(tempFilesName);
    fclose(totalWord);

    tempFilesName=fopen("logPaths.txt","r");
 	totalWord=fopen("wordOfFiles.txt","r");

 		status=fscanf(totalWord,"%d",&fileCount);
 		while(status!=EOF){
 			totalCount+=fileCount;
 			status=fscanf(totalWord,"%d",&fileCount);
 		}

 		remove("wordOfFiles.txt");

 		status=fscanf(tempFilesName,"%s",filePath);
 		while(status!=EOF){
 			char ch;
 			FILE*temp;
 			temp=fopen(filePath,"r");
 			for(;fscanf(temp,"%c",&ch)!=EOF;)
 				fprintf(listFromFileLog,"%c",ch);
 			fclose(temp);
 			remove(filePath);
 		 status=fscanf(tempFilesName,"%s",filePath);
 		}

 		remove("logPaths.txt");
    fprintf(listFromFileLog,"---------------\nSearched : %s  found %d times",argv[1],totalCount);

 	fclose(listFromFileLog);
 		
    

    fprintf(stderr,"Searched : %s  found %d times",argv[1],totalCount);

return 0;
}

void listFromFile(char* fileName, char* string, FILE*tempFileNames,FILE* totalWord){


int i,j,status,line=1,column=0;
int startIndex=0,endIndex=0;
char* tempLogPath;
char temp;
char* buff;
int count=0;
FILE* input;
FILE* tempLog;

	input=fopen(fileName,"r");

	tempLogPath=(char*)malloc((strlen(fileName)+5)*sizeof(char));
	strcpy(tempLogPath,fileName);
	tempLogPath=strtok(tempLogPath,".");
	strcat(tempLogPath,".log");
 	tempLog=fopen(tempLogPath,"w");
	

	fflush(tempFileNames);
	
	fprintf(stderr, " == my pid : %d   my parent pid:%d  file path: %s  temp log opened ==\n",getpid(),getppid(),fileName);
	if(input==NULL)
	{
		perror("File is not existing!!");
	}	

	if(tempLog == NULL){
		fprintf(stderr, "==%d== Cannot open templog file.\n\t%s\n", getpid() ,strerror(errno));
	}

	/*fprintf(stderr, " geldi mi la buraya \n");*/
	fprintf(tempLog,"\n------------------------\nFile Name : %s   string : %s \n",fileName,string);

	 buff=(char*)malloc(strlen(string)*sizeof(char)+1);

	/*status=fscanf(input,"%c",&temp);*/

    buff[0]=temp;

	/*fprintf(stderr, " geldi mi la buraya ilk okuma \n");*/


	do {
		status = fscanf(input, "%c", &temp);
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
				status = fscanf(input, "%c", &temp);
				if (temp != ' ' && temp != '\n' && temp != '\t') {
					buff[len - diff] = temp;
					--diff;
					/*fprintf(stderr, "%s\n", buff);*/
				}
				++column;
				endIndex++;
			}

			fseek(input, -(int) (endIndex - startIndex), SEEK_CUR); /* to find sequential words */

			if (strcmp(string, buff) == 0) {
				count++;

				column -= (endIndex - startIndex);
				fprintf(stderr, "<file : %s [%d,%d] string:%s > \n", fileName,line,
						column, string);
				fprintf(tempLog, "<file : %s [%d,%d] string:%s > \n", fileName,line,
						column, string);
				endIndex = startIndex;
			}


			for (j = strlen(buff) - 1; j >= 0; --j)
				buff[j] = '\0';
		}
	} while (status != EOF);

		fprintf(tempFileNames,"%s\n",tempLogPath);
		fprintf(stderr,"%s\n",tempLogPath);
		free(tempLogPath);


		fprintf(totalWord,"%d\n",count);
		fclose(tempLog);
		fclose(input);

}


int openningDirectories(char* directory, FILE* tempFileNames,FILE* totalWord,char*string){
	
	pid_t childProcess; /*Cocuk mu degil mi diye bakarken kullanilacak*/
	int status; /*Wait fonksiyonuna buffer saglamak icin*/
	DIR *next; /*Klasor pointer*/
	char * absolutePath;
	struct dirent *myfile;/*Dosya bilgilerini tutan struct.*/
	struct stat statFile;/*Klasor mu degil mi diye bakiyor*/
	



	if ((next = opendir (directory)) != NULL){
		while ((myfile = readdir (next)) != NULL){
			if(strcmp(myfile->d_name, ".")!=0 && strcmp(myfile->d_name, "..")!=0){

				absolutePath=(char*)malloc((strlen(directory)+strlen(myfile->d_name)+1)*sizeof(char));
				strcpy(absolutePath, directory);
				strcat(absolutePath, "/");
				strcat(absolutePath, myfile->d_name);/*Dosyanin tam adresini olusturur.*/

				if (stat(absolutePath, &statFile) == 0 && S_ISDIR(statFile.st_mode)){/*Eger klasor varsa recursive kisim baslar*/
				   openningDirectories(absolutePath,tempFileNames,totalWord,string);
				}

				if((int)strlen(myfile->d_name)>4 && strcmp(myfile->d_name+((int)strlen(myfile->d_name)-4), ".txt")==0){/*Txt dosyasi bulursa parent bir child olusturur.*/
					childProcess=fork();
					if(childProcess == -1){
						fprintf(stderr, "FORK FAILED ==%d==\n", getppid());
						_exit(1);
					}
				if(childProcess==0){/*Sadece cocuk girer*/
					fprintf(stderr,"==%d== Child process entered\n",getpid());
					fprintf(stderr,"==%d== grepFromFile arguments :\n\tabsolutePath : %s\n\tword : %s\n",getpid(),absolutePath,string);
					listFromFile(absolutePath, string , tempFileNames,totalWord);/*arama islemini yapar*/
					fprintf(stderr,"==%d== Child process exit\n",getpid());
					exit(1);/*ve olur.*/
				}
				
				wait(&status);/*Bu sirada parent baska bir cocuk olusturmasin diye bir onceki cocugun olmesini bekler... Cocuk bu satira hic bir zaman ulasamaz.*/
				

				}
			}
		}
	}
	else{
		
		exit(1);
	}
	free(absolutePath);
	closedir(next);

}
