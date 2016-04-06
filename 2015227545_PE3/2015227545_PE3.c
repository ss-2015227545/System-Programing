#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct tagNode{
        char * nofNode;
        struct tagNode *next;
}NODE;

NODE *head, *tail, *working;
ino_t get_inode(char *);
void pathto(ino_t);
void inum_to_name(ino_t, char *, int);
void InitList();
void Insert(char*string);
void RemoveAll();
void DisplayList();
NODE *Inverse(NODE *);


int main()
{
        InitList();
        pathto( get_inode("."));
        DisplayList(Inverse(head));
        RemoveAll();
        putchar('\n');
        return 0;
}

ino_t get_inode( char *fname)
{
        struct stat info;
        if( stat( fname, &info) == -1){
                fprintf(stderr, "Cannot stat");
                perror(fname);
                exit(1);
        }
        return info.st_ino;
}

void pathto( ino_t this_inode)
{
        ino_t my_inode;
        char its_name[BUFSIZ];
        if( get_inode("..") != this_inode ){
                chdir("..");
                inum_to_name( this_inode, its_name, BUFSIZ);
                my_inode = get_inode(".");
                Insert( its_name );
        }
        while(get_inode("..") != my_inode ){
                chdir("..");
                inum_to_name( my_inode, its_name, BUFSIZ);
                my_inode = get_inode(".");
                Insert(its_name);
        }
}


void inum_to_name( ino_t inode_to_find, char *namebuf, int buflen )
{
        DIR             *dir_ptr;
        struct dirent   *direntp;

        dir_ptr = opendir( "." );
        if( dir_ptr == NULL ){
                perror(".");
                exit(1);
        }

        while( ( direntp = readdir( dir_ptr ) ) != NULL)
                if( direntp-> d_ino == inode_to_find){
                        strncpy( namebuf, direntp->d_name, buflen );
                        namebuf[buflen-1] = '\0';
                        closedir( dir_ptr );
                        return;
                }
        fprintf( stderr, "error looking for inum %d\n", inode_to_find);
        exit(1);
}

void InitList()
{
        head = NULL;
        tail = NULL;
        working = NULL;
}

void Insert(char *string)
{
        working = (NODE*)malloc(sizeof(NODE));
        working->nofNode = (char *)malloc(strlen(string));
        strcpy(working->nofNode, string);

        working->next =NULL;

        if(head ==NULL){
                head = working;
                tail = working;
                return;
        }

        tail->next = working;
        tail = working;
}
void DisplayList(NODE *working)
{
        while(working){
                printf( "%s ", working->nofNode);
                working = working->next;
        }
}


void RemoveAll()
{
        working = head;

        while(working){
                NODE*node = working;
                working = working->next;
                free(node);
                node = NULL;
        }
        InitList();
}
NODE *Inverse(NODE *head)
{
        working = NULL;
        while(head){
                tail = working;
                working = head;
                head = head->next;
                working->next = tail;
        }
        return working;
}
