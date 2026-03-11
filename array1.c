#include  <stdio.h>

int main() {

    int a[3] = {1,2,3};

    int *b = a + sizeof(a)/sizeof(int);

    // printf("primo numero %d",*(a));
    // printf("secondo numero %d",*(a+1));
    // printf("terzo numero %d",*(a+2));

    printf("%d",*(b-1));




}