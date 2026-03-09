#include <stdio.h>
#include <stdlib.h>

int main() {
    int *p; // dichiaro un puntatore ad interi
    p = (int *) malloc(sizeof(int)*4); //malloc mi ritornera l'indirizzo di memoria di un aerea grande come 4 interi

    //li sfrutto un po come un array ma senza usare un array
    for (int i=0; i<4; i++) {
        *(p + i) = i;
    }

    for (int i=0; i<4; i++) {
        printf("%d\n",*(p + i));
    }
}
