#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int get_ok_char();

void main()
{
        void f(int);
        int i;
        signal(SIGINT,f);
        /*ile(1)*/
        for(i = 0 ; i < 10 ; i++){
                printf("hellow\n");
                sleep(1);
        }
}

void f(int signum){
        printf("  Interrupted! Ok to quit(y/n): ");
       	int input= tolower(get_ok_char());
        if(input == 'y')
                exit(0);
}

int get_ok_char(){
        int c;
        while ((c = getchar()) != EOF && strchr("yYnN",c) == NULL);
        return c;
}

