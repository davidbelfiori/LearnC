//
// Created by davidjulianbelfiori on 03/07/26.
//

/*
* SPECIFICATION TO BE IMPLEMENTED:
Si sviluppi una applicazione che riceva tramite argv[] la seguente linea di comando

    nome_prog -f file1 [file2] ... [fileN] -s stringa1 [stringa2] ... [stringaN]

indicante N nomi di file (con N > 0) ed N ulteriori stringhe (il numero dei nomi dei
file specificati deve corrispondere al numero delle stringhe specificate).

L'applicazione dovra' generare N processi figli concorrenti, in cui l'i-esimo di questi
processi effettuera' la gestione dell'i-esimo dei file identificati tramite argv[].
Tale file dovra' essere rigenerato allo startup dell'applicazione.
Il main thread del processo originale dovra' leggere indefinitamente stringhe da
standard input e dovra' comparare ogni stringa letta che le N stringhe ricevute in
input tramite argv[].
Nel caso in cui la stringa letta sia uguale alla i-esima delle N stringhe ricevuta
in input, questa dovra' essere comunicata all'i-esimo processo figlio in modo che questo
la possa inserire in una linea del file di cui sta effettuando la gestione. Invece,
se il main thread legge una stringa non uguale ad alcuna delle N stringhe ricevute
in input, questa stringa dovra essere comunicata a tutti gli N processi figli
attivi, che la dovranno scrivere sui relativi file in una nuova linea.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando uno qualsiasi dei processi figli venga colpito
dovra' riportare il contenuto del file da esso correntemente gestito in un file
con lo stesso nome ma con suffisso "_backup".  Invece il processo originale non dovra'
terminare o eseguire alcuna attivita' in caso di segnalazione.

In caso non vi sia immissione di dati sullo standard input e non vi siano segnalazioni,
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.
 */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024


volatile sig_atomic_t sigCatch = 0 ;

void handler  (int sig) {

    sigCatch = 1;
}

void messaggio(char *msg) {
    printf("errore: %s",msg );
    exit(EXIT_FAILURE);
}

void eseguiBackUp(char *nomeFile) {
    char *nome = nomeFile;
    printf("SIGINT RILEVATA ESEGUO BACKUP, lo potrai trovae in questo file backup_%s \n",nome);

    int fdSorg = open(nome,O_RDONLY);
    if (fdSorg <0) messaggio("errore nell'apertura del file sorg \n");
    char backupName[512];
    snprintf(backupName,sizeof(backupName),"%s_backup",nome);
    int fdBack = open(backupName,O_CREAT|O_TRUNC|O_WRONLY,0644);
    if (fdBack <0) {
        close(fdSorg);
        messaggio("errore nell'apertura del file back\n");
    }

    ssize_t byteLetti = 0;
    char buff[1024];
    while ((byteLetti = read(fdSorg,buff,sizeof(buff)))>0) {
        write(fdBack,buff,byteLetti);
    }
    close(fdSorg);
    close(fdBack);
    printf("Terminato \n");

}

int main(int argc , char **argv) {

    char **fileName = malloc(argc * sizeof(char *));
    char **strings = malloc(argc*sizeof(char *));
    int num_files = 0;
    int num_strings = 0;

    int mode = 0;
    for (int i = 1; i<argc; i++) {

        if (strcmp(argv[i],"-f") == 0) {
            mode =1;
            continue;
        }else if (strcmp(argv[i],"-s") == 0) {
            mode = 2;
            continue;
        }

        if (mode == 1) {
            fileName[num_files++] = argv[i];
        }else if(mode == 2) {
            strings[num_strings++] = argv[i];
        }
    }

    if ((num_files != num_strings )|| num_files == 0 ||num_strings == 0) {
        messaggio("errore uso : nome_prog -f file1 [file2] ... [fileN] -s stringa1 [stringa2] ... [stringaN] \n");
    }


    int N = num_files;
    int pipes[N][2];

    signal(SIGINT, SIG_IGN);

    for (int i = 0; i<N;i++) {
        if (pipe(pipes[i])==-1) messaggio("errore nella creazione della pipe");

        pid_t pid= fork();
        if (pid < 0)messaggio("errore nella creazione del figlio");
        if (pid == 0) {

            for (int j = 0; j<=i;j++) {
                close(pipes[j][1]);
                if (j<i) {
                    close(pipes[j][0]);
                }
            }

            int miaPipe = pipes[i][0];
            char *mioFile = fileName[i];

            struct sigaction sa;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);
            sa.sa_handler=handler;
            sigaction(SIGINT,&sa,NULL);

            int fdDest = open(mioFile,O_CREAT|O_TRUNC|O_WRONLY,0666);
            if (fdDest<0) messaggio("Impossibile aprire il file");

            while (1) {
                if (sigCatch ==1) {
                    //abbiamo ricevuto una sigint
                    eseguiBackUp(mioFile);
                    sigCatch=0;
                }else {
                    //non abbiamo ricevuto una sigint e possiamo rimanere in attesa della pipe
                    char c;
                    ssize_t byteRead = read(miaPipe,&c,1);
                    if (byteRead == -1) {
                        if (errno == EINTR) {sigCatch = 1; continue;}else{break;}
                    }

                    if(c == '\0') {
                        write(fdDest, "\n", 1); // La stringa è finita, vai a capo sul file
                    } else {
                        write(fdDest, &c, 1);   // Scrivi il singolo carattere direttamente
                    }

                }

            }
            close(miaPipe);
            close(fdDest);
            exit(EXIT_SUCCESS);
        }
    }

    //PROCESSO PADRE
         for (int i= 0; i<N; i++) {
             //chiudo la lettura a tutti i figli
             close(pipes[i][0]);
         }
            char buff[MAX_LINE];
            while (fgets(buff,MAX_LINE,stdin)!= NULL) {

                //cerco lo /n e metto /o
                buff[strcspn(buff,"\n")]='\0';


                int match= 0;

                for (int j = 0;j<N;j++) {
                    //la tringa nel buffer è uguale a quello che ho ricevuto da cli ?
                    if (strcmp(buff,strings[j])==0) {
                        write(pipes[j][1],buff,strlen(buff)+1);
                        match = 1;
                        break;
                    }
                }
                //se dopo il for non ho trovato niente la mando a tutti
                if (match==0) {
                    for (int j =0 ; j<N; j++) {
                        write(pipes[j][1],buff,strlen(buff)+1);
                    }
                }
            }


    for (int i =0 ; i<N; i++) {
        close(pipes[i][1]);
    }

    for (int i =0 ; i<N; i++) {
        wait(nullptr);
    }

    free(fileName);
    free(strings);

}