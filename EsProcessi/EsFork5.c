#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_TOKENS 128

int main(int argc, char** argv) {
    char buffer[4096];//BUFFER DELLE RIGA
    char *p;
    gets(buffer);
    p = (char*)strtok(buffer,s);




}