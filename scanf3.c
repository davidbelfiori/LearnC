#include <stdio.h>

int main(int argc , char *argv[]) {
    char p[3];
    if (scanf("%s",p)==1) {
        printf("ok, p: %s",p);
    }else {
        printf("errore");
    };
}