/*
 *webserv.c - a minimal webserver(version 0.2
 *usage : ws portnumber
 *featurees : supports the GET command only
 *	run in the current directory
 *	fork a new child to haldles each request
 * 	has MAJOR security holes, for demo purposes only
 * 	hase many other weaknesses, bbut is a good start
 * 	build : cc webserv.c soklib.c -o webserv
*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main( int ac, char *av[])
{
	int sock, fd;
	FILE *fpin;
	char request[BUFSIZ];
	
	if( ac == 1 ){
		fprintf( stderr, "usage : ws protnum\n" );
		exit(1);
	}

	sock = make_server_socket( atoi( av[1] ) );
	if( sock == -1 )	exit(2);
	
	/*main loop here*/
	while(1){
		/*take a call and buffer it*/
		fd = accept( sock, NULL, NULL );
		fpin = fdopen( fd, "r" );
	
		/*read request*/
		fgets( request, BUFSIZ, fpin );
		printf( "got a call: request = %s", request );
		read_til_crnl( fpin );

		/*do what client asks*/
		process_rq(request, fd);

		fclose(fpin);
	}
	return 0;
} 

/*-------------------------------------------------
  read_til_crnl(FILE*)
  skip over all request info until a CRNL is seen
 --------------------------------------------------*/

read_til_crnl(FILE *fp){
	char buf[BUFSIZ];
	while( fgets( buf, BUFSIZ, fp ) != NULL && strcmp( buf, "\r\n") != 0 );
}

/*------------------------------------------------
  process_rq( char * rq, int fd )
  do what the request asks for and  write reply to fd
  handles request in a new process
  rq is HTTP command: GET/foo/bar.html HTTP/1.0
 -------------------------------------------------*/

process_rq( char *rq, int fd ){
 	char cmd[BUFSIZ], arg[BUFSIZ];
	FILE *fp;
	fp = fdopen( fd, "w" );

	/*create a new process and reutrn if not the child*/
	if( fork() != 0 )
		return;

	strcpy( arg, "./;" ); // preced args with ./
	if( sscanf( rq, "%s%s", cmd, arg+2 ) != 2 )
		return;  
	printf( "%s\n%s\n", cmd, arg );
	if( strcmp( cmd, "HEAD" ) != 0 )
		cannot_do( fd );
	else if( not_exist( arg ) )
		do_404( arg,fd );
	else if( isadir( arg ) )
		do_ls( arg, fd );
	else if( ends_in_cgi( arg ) )
		do_exec( arg, fd );
	else
		do_cat( arg, fd );
}

/*-------------------------------------------------------
  the reply header thing: all functions need one
  if contennt_type is NULL then don't send content type
---------------------------------------------------------*/

header( FILE *fp, char *content_type ){
	fprintf( fp, "HTTP/ 1.0 200 OK\r\n");
	if( content_type )
		fprintf( fp, "Content-type: %s\r\n", content_type );
}

/*---------------------------------------------------------
  simple functions first
	cannot_do( fd ) 	unimplemented HTTP command
	and do_404( item, fd )	no such object
----------------------------------------------------------*/

cannot_do( int fd ){
	FILE *fp = fdopen( fd, "w" );
	
	fprintf( fp, "HTTP/1.0 501 Not Implemented\r\n" );
	fprintf( fp, "Content-type: text/plain\r\n" );
	fprintf( fp, "\r\n" );
	
	fprintf( fp, "That command is not yet implemented\r\n" );
	fclose( fp );
}

do_404( char *item, int fd ){
	FILE *fp = fdopen( fd, "w" );

	fprintf( fp, "HTTP/1.0 404 Not Found\r\n" );
	fprintf( fp, "Content-type: text/plain\r\n" );
	fprintf( fp, "\r\n" );
	
	fprintf( fp, "The item you requested: %s\r\nis not found\r\n", item );
	fclose( fp );
}

/*-------------------------------------------------------------
  the directory listing section
  isadir() uses stat, not_exist() uses stat 
  do_ls runs ls. It should not
-------------------------------------------------------------*/

isadir( char * f ){
 	struct stat info;
	return( stat(f, &info) != -1 && S_ISDIR( info.st_mode ) );
}

not_exist( char *f ){
	struct stat info;
	return( stat( f, &info ) == -1 );
}

do_ls( char *dir, int fd ){
	FILE * fp;
	
	fp = fdopen( fd, "w" );
	header( fp, "text/plain" );
	fprintf( fp, "\r\n" );
	fflush( fp );

	dup2( fd, 1 );
	dup2( fd, 2 );
	close( fd );
	execlp( "ls", "ls", "-l", dir, NULL );
	perror( dir );
	exit(1);
}

/*-------------------------------------------------------------------
  the cgi stuff. fuction to check extension and 
  one to run the program.
--------------------------------------------------------------------*/
char *file_type(char* f){
	/* return 'extension' of file */
	char *cp;
	if( ( cp = strrchr( f, '.' ) ) != NULL )
		return cp + 1;
	return "";
}

ends_in_cgi( char *f ){
	return ( strcmp( file_type( f ), "cgi" ) == 0 );
} 

do_exec( char *prog, int fd ){
	FILE *fp;
	
	fp = fdopen( fd, "w" );
	header(fp, NULL );
	fflush( fp );
//	dup2( fd, 1 );
//	dup2( fd, 2 );
//	close( fd );
//	execl( prog, prog, NULL );
//	perror( prog );
}

/*--------------------------------------------------------------
  do_cat( filename, fd )
  sends back contets after a header
--------------------------------------------------------------*/

do_cat( char *f, int fd ){
	char *extension = file_type( f );
	char *content = "text/plain";
	FILE *fpsock, *fpfile;
 	int c;
	
	if( strcmp( extension, "html" ) == 0 )
		content = "text/html";
	else if( strcmp( extension, "gif" ) == 0 )
		content = "image/gif";
	else if( strcmp( extension, "jpg" ) == 0 )
		content = "image/jpg";
	else if ( strcmp( extension, "jpeg" ) == 0 )
		content = "image/jpeg";

	fpsock = fdopen( fd, "w" );
//	fpfile = fopen( f, "r" );
	
	if( fpsock != NULL &&fpfile != NULL ){
		header( fpsock, content );
		fprintf( fpsock, "\r\n" );
/*		while( ( c = getc( fpfile ) ) != EOF )
			putc( c, fpsock );  		*/a
		fclose( fpfile );
//		fclose( fpsock );
	}
	exit(0);
}
