//
// Created by davidjulianbelfiori on 05/03/26.
//

#include <stdio.h>
#define CLEAN_STDIN() do { \
int c; \
while ((c = getchar()) != '\n' && c != EOF); \
} while (0)

int main () {
    int x;
    int returnvalue1, returnvalue2;

    printf("Inserisci un numero: ");
    returnvalue1 = scanf("%d", &x);
    printf("Valore di ritorno della prima scanf: %d\n", returnvalue1);

    //Metodo per pulire
    if (returnvalue1 == 0) {
        // Pulizia manuale del buffer: legge tutto finché non trova l'invio
       CLEAN_STDIN();
        printf("Buffer pulito.\n");
    }

    printf("Prova a inserire di nuovo un numero: ");
    returnvalue2 = scanf("%d", &x);
    printf("Valore di ritorno della seconda scanf: %d\n", returnvalue2);

    return 0;
}
/* input : a
* output: a
valore di ritorno della prima scanf 0
valore di ritorno della seconda scanf 0
*/