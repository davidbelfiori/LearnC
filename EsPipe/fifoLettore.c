#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {

    char buff[1024];
    int byteRead = 0;

    while ((byteRead = read(0,buff,sizeof(buff)))>0) {

        printf("%s",buff);


    }

}