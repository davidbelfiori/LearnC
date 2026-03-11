#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // Aggiunta per usare bool
#define ffush(stdin) (while (getchar() != '\n');)


typedef struct {
    char name[20];
    int age;
} anagrafica;

int main() {
    anagrafica *p;
    // Allochiamo spazio per 4 strutture
    p = (anagrafica *)malloc(sizeof(anagrafica) * 4);

    if (p == NULL) return 1; // Controllo sicurezza malloc

    for (int i = 0; i < 4; i++) {
        // --- Lettura ETÀ ---
        bool verifica_eta = false;
        while (!verifica_eta) {
            printf("\nInserisci l'eta' per la persona %d: ", i + 1);
            if (scanf("%d", &(p + i)->age) == 1) {
                verifica_eta = true; // Esci se l'input è un numero
            } else {
                printf("Errore, inserisci un numero valido.\n");
                fflush(stdin); // Pulisce il buffer da caratteri errati
            }
        }

        // --- Lettura NOME ---
        printf("Inserisci il nome: ");
        scanf("%s", (p + i)->name); // Niente & per le stringhe
    }

    // Stampa di verifica
    printf("\n--- Dati Inseriti ---\n");
    for (int i = 0; i < 4; i++) {
        printf("%d) Nome: %s, Eta': %d\n", i + 1, (p + i)->name, (p + i)->age);
    }

    free(p); // Ricordati sempre di liberare la memoria!
    return 0;
}