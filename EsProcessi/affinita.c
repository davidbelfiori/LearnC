#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
int main() {
        cpu_set_t mask;
        pid_t pid = getpid(); // ID del processo corrente
        // Inizializza la maschera a 0 e imposta solo la CPU 0
        CPU_ZERO(&mask);
        CPU_SET(0, &mask);
         CPU_SET(1, &mask);
        // Imposta l'affinità
        if (sched_setaffinity(pid, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        return EXIT_FAILURE;
        }

        printf("CPU consentite:");
        for (int i = 0; i < CPU_SETSIZE; i++)
        {
            if(CPU_ISSET(i,&mask)){
                printf("%d",i);
            }
        }
        


        printf("Affinità impostata: processo %d vincolato a CPU 0\n", pid);
        // Recupera l'affinità corrente
        CPU_ZERO(&mask);
        if (sched_getaffinity(pid, sizeof(mask), &mask) == -1) {
        perror("sched_getaffinity");
        return EXIT_FAILURE;
        }
        printf("CPU consentite: ");
        for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &mask)) {
        printf("%d ", i);
        }
        }
        printf("\n");
        return EXIT_SUCCESS;
}