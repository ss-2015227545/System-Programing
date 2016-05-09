#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXARGS 20 //cmdline args
#define ARGLEN 100 //token length

int main()
{
	char 	*arglist[MAXARGS + 1];	//AN ARRAY OF PTRS
	int 	numargs;		//index into array
	char 	argbuf[ARGLEN]; 	// read stuff here
	char 	*makestring();

	numargs = 0;
	
	while( numargs < MAXARGS ){
		printf("Arg[%d]?",numargs);
		if( fgets( argbuf, ARGLEN, stdin ) && *argbuf != '\n'){
			if( *argbuf == EOF )
				return 0;
			arglist[numargs++] = makestring(argbuf);
		}
		else if( numargs > 0 ){	//any args?
			arglist[numargs] = NULL; 	//close list
			execute( arglist );		//do it
			numargs = 0; 			//and reset
		}
	}
	return 0;
}


/*
*	use fork and execvp and wait to do it
*/
void execute( char *arglist[] ){
	int pid, exitstatus; 	//of child
	void f( int );
	pid = fork();		//make new process
	switch(pid){
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			execvp( arglist[0], arglist );
			perror("execvp failed");
			exit(1);
		default:
			signal( SIGINT,SIG_IGN );
			while( wait( &exitstatus ) != pid )
				;
			signal( SIGINT,SIG_DFL );
			printf("\n");
		//	printf("child exited with status %d, %d\n", exitstatus>>8,exitstatus&0377);
	}
}

/*
*	trim off newline and create storage for the string
*/
char *makestring( char *buf ){
	char	*cp, *malloc();
	
	buf[strlen( buf ) - 1] = '\0';		//trim newline
	cp = malloc( strlen( buf ) + 1 );	//get memory
	if( cp == NULL ){			//or die
		fprintf( stderr, "no memory\n" );
		exit(1);
	}
	strcpy( cp, buf );		//copy chars
	return cp;			//return ptr
}

