//
// Created by davidjulianbelfiori on 02/07/26.
//
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Scrivere un programma che riceva in input tramite argv[] il nome di un file
F e una stringa S contenente un numero arbitrario di caratteri.
Il main thread dell'applicazione dovra' creare il file F e poi leggere indefinitamente
stringhe dallo standard input per poi scriverle, una per riga, all'interno del file.
Qualora venga ricevuto il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI), dovra'
essere lanciato un nuovo thread che riporti il contenuto del file F all'interno di un
altro file con lo stesso nome, ma con suffisso "_shadow", sostituendo tutte le stringhe
che coincidono con la stringha S ricevuta dall'applicazione tramite argv[] con
una stringha della stessa lunghezza costituita da una sequenza di caratteri '*'.
Il lavoro di questo thread dovra' essere incrementale, ovvero esso dovra' riportare
sul file shadow solo le parti del file originale che non erano state riportate in
prcedenza. Al termine di questa operazione, il thread dovra' indicare su standard
output il numero di stringhe che sono state sostituite in tutto.
 */

volatile sig_atomic_t sigint_flag  = 0;
char *nomeFile;
char *parola ;
char *nomeShadow;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
off_t offsetLetto = 0;
int sostituzioni_totali = 0;


void messaggio(char *msg) {
    printf("errore: %s \n",msg);
    exit(EXIT_FAILURE);
}

void handler(int sig) {
    sigint_flag =1;
};

void *work(void *arg) {
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);

    pthread_sigmask(SIG_BLOCK,&sig,NULL);
    pthread_mutex_lock(&mutex);

    // 3. Apertura file descriptor con system call
    int fd_in = open(nomeFile, O_RDONLY);
    int fd_out = open(nomeShadow, O_CREAT | O_WRONLY | O_APPEND, 0666);

    if (fd_in < 0 || fd_out < 0) {
        printf("Errore nell'apertura dei file nel thread.\n");
        if (fd_in >= 0) close(fd_in);
        if (fd_out >= 0) close(fd_out);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // 4. Riposizionamento dell'offset
    lseek(fd_in, offsetLetto, SEEK_SET);

    char c;
    char bufferParola[1024];
    //indice parola;
    int idx = 0;
    size_t lenParola = strlen(parola);
    int sostituzioni_locali = 0;

    // Buffer preparato con gli asterischi per scriverlo in un colpo solo
    char bufferAsterischi[1024];
    memset(bufferAsterischi, '*', lenParola);

    // 5. Lettura carattere per carattere
    while (read(fd_in, &c, 1) > 0) {
        // Se trovo un separatore (spazio, a capo o tab), significa che la parola è finita
        if (c == ' ' || c == '\n' || c == '\t') {
            //vuol dire che la stringa è finita
            if (idx > 0) {
                //aggiungo il terminatore
                bufferParola[idx] = '\0'; // Termino la stringa

                if (strcmp(bufferParola, parola) == 0) {
                    // La parola coincide: scrivo gli asterischi
                    write(fd_out, bufferAsterischi, lenParola);
                    sostituzioni_locali++;
                } else {
                    // La parola non coincide: la riscrivo uguale
                    write(fd_out, bufferParola, idx);
                }
                idx = 0; // Resetto l'indice per la prossima parola
            }
            // Scrivo il carattere separatore (spazio o a capo) per mantenere la formattazione
            write(fd_out, &c, 1);
        } else {
            // Accumulo i caratteri nel buffer, evitando overflow
            if (idx < sizeof(bufferParola) - 1) {
                bufferParola[idx++] = c;
            }
        }
    }

    // Caso limite: se il file finisce senza un a capo/spazio finale
    if (idx > 0) {
        bufferParola[idx] = '\0';
        if (strcmp(bufferParola, parola) == 0) {
            write(fd_out, bufferAsterischi, lenParola);
            sostituzioni_locali++;
        } else {
            write(fd_out, bufferParola, idx);
        }
    }

    // 6. Salvataggio del nuovo offset e chiusura
    offsetLetto = lseek(fd_in, 0, SEEK_CUR);
    sostituzioni_totali += sostituzioni_locali;

    printf("\n[THREAD] Lavoro incrementale (via read/write) terminato.\n");
    printf("Sostituzioni locali: %d. Sostituzioni totali: %d\n", sostituzioni_locali, sostituzioni_totali);

    close(fd_in);
    close(fd_out);
    pthread_mutex_unlock(&mutex);

    return NULL;

}

int main(int argc, char *argv[]) {

    if (argc < 3)messaggio("Uso ./nomeProgramma nomefile Parola");

    nomeFile = argv[1];
   parola = argv[2];

    nomeShadow = malloc(strlen(nomeFile)+10);
    sprintf(nomeShadow,"shadow_%s",nomeFile);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0 ;
    sa.sa_handler= handler;
    if (sigaction(SIGINT,&sa,NULL)==-1) messaggio("errore nella creazione della sigaction");

    int srcFd = open(nomeFile,O_CREAT|O_APPEND|O_TRUNC|O_RDWR,0666);
    if (srcFd<0) messaggio ("Errore nell'apertura del file");

    printf("Inzio lettura da stdin \n");
    char buff[1024];

    while (1) {

        if (sigint_flag == 1) {
            sigint_flag = 0;
            printf("catturata SIGINT lancio thread \n");
            pthread_t tid;
           if ( pthread_create(&tid,NULL,work,NULL)==0) {
               pthread_detach(tid);
           }else {
               messaggio("errrore nella creazione del thread");
           }

        }else {
            ssize_t byteRead = read(0, buff, sizeof(buff));
            if (byteRead>0) {
                write(srcFd,buff,byteRead);
            }else if (byteRead < 0) {
                if (errno == EINTR) {
                    continue;
                }else {
                    messaggio("errore write");
                }
            }
        }
    }


}