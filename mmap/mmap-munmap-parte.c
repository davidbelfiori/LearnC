#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define  PAGE_SIZE  4096
int main(int argc,char *argv[]) {
    //* cosa andreamo a vedere -> dato un address space della nosntra applicazione , andremo a chiedere al nonstro sistema operativo di
    //*fornirci un aria di memoria aggiuntiva "di mapparci una zona di memeoria grigia" composta da 2 pagine e poi andremo a smontarne una delle due

    char *ptr = mmap(NULL,2*PAGE_SIZE,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,0,0);
    if (ptr == NULL) {
        exit(0);
    }
    char *x1 = ptr;
    char *x2 =ptr + PAGE_SIZE;

    printf("indirizzo di x1 %p \n",(void *)x1);
    printf("indirizzo di x2 %p \n",(void *)x2);

    x1[0]='L';
    x1[1]='\0';

    x2[0]='A';
    x2[1]='\0';

   if (munmap(ptr,PAGE_SIZE) == -1) exit(0) ;

    printf("valore interno a x2 %c \n",x2[0]);

    return 0;



}