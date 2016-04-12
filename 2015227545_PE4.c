#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


int main(int argc, char *argv[])
{
	DIR *dir; 
	char temp[100] = {};

	if( argc != 3){
		printf("error");
		exit(0);
		}

	dir = opendir(argv[2]);

	if(dir){
		strcpy(temp,argv[2]);
		strcat(temp,"/");
		strcat(temp,argv[1]);
		rename(argv[1], temp);		
		printf("move directory  %s\n",argv[2]);
	}
	else{
		if( strcmp(argv[1], argv[2])== 0){
			printf("%s and %s  area the same file\n", argv[1], argv[2]);
			return 0;
		}
		rename(argv[1], argv[2]);
		printf("rename %s\n", argv[2]);
	}
	
	return 0;
}

