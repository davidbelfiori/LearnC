#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
char c;
int x, y, i;


void gestioneTimeout(int signum) {
    printf("I'm alive! x = %d, y = %d\n", x, y);
    alarm(1);
}


int main(int argc, char *argv[]) {
    signal(SIGALRM, gestioneTimeout);
    alarm(1);
    while(1) x = y = i++ % 100;
}