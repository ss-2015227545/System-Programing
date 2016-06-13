#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define oops(m, x) {perror(m), exit(x); }

int main(int argc, char* argv[])
{
	int apipe[2], bpipe[2], pid1, pid2;

	if( argc != 4){
		perror("not match argc\n");
		exit(1);
	}

	if ( pipe(apipe)==-1)
		oops("Cannot get a pipe", 1);

	if( (pid1= fork()) == -1)
		oops("Cannot fork",2);

	if( pid1 > 0){
		close(apipe[1]);
		if (dup2(apipe[0], 0) == -1)
			oops("Could not redirect stdin", 3);
		close(apipe[0]);
		execlp(argv[3],argv[3], NULL);
		oops(argv[3],4);
	}

	else{
		if(pipe(bpipe) == -1)
			oops("Cannot get a pipe2", 5);
		if((pid2 = fork()) == -1)
			oops("Cannot fork2", 6);
		if(pid2 > 0){	
			close(apipe[0]);
			if(dup2(apipe[1],1) == -1)
				oops("Cannnot redirect stdout",7);
			close(apipe[1]);

			close(bpipe[1]);
			if( dup2(bpipe[0], 0) == -1)
				oops("Could not redirect stdin2", 8);
			close(bpipe[0]);
			execlp(argv[2],argv[2],NULL);
			oops(argv[2],9);
		}
		else{		
			close(bpipe[0]);
			if(dup2(bpipe[1],1) == -1)
				oops("Cannot redirect stdout2", 10);
			close(bpipe[1]);
				execlp(argv[1], argv[1], NULL);
			oops(argv[1],11);
		}
	} 
	
	return 0;
}

