
/******************************************************************
SPECIFICATION TO BE IMPLEMENTED:
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

*****************************************************************/
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


volatile sig_atomic_t sigErr = 0;
int N;
char **file;

void messaggio(char *m) {
    printf("Errore: %s \n",m);
    exit(EXIT_FAILURE);
}

void handler(int sig) {
    printf("Catturato \n");
    sigErr = 1;
}


void backUp(void) {


    for (int i = 0; i < N; i++) {
        printf("---------Inizio copia per il file %s --------- \n",file[i]);
        char bFile[512];
        snprintf(bFile,512,"%s_backup",file[i]);

        int fdSorg = open(file[i],O_RDONLY,0644);
        int fdDest = open(bFile,O_CREAT|O_WRONLY|O_TRUNC,0666);

        if (fdSorg < 0 || fdDest <0) messaggio (" errore file backup");

        ssize_t byteRead  = 0;
        char buff[1024];
        while ((byteRead = read(fdSorg,buff,sizeof(buff)))>0){
        write(fdDest,buff,byteRead);
        }
        printf("---------fine --------- \n");
        close(fdSorg);
        close(fdDest);
    }



}

void sonWork(int fdPipe, char *file, char *str) {
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler= handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);

    int fd= open (file,O_RDWR|O_CREAT|O_TRUNC,0666);
    if (fd <0) messaggio("apertura file figlio");

    char buffer[1024];
    int idx = 0;
    while (1) {
        if (sigErr==1) {
            backUp();
            sigErr = 0;
        }else {
            char c;
            ssize_t r = read(fdPipe,&c,1);

            if (r == 0) break;
            if (r == -1) break;

            if (c =='\0') {
                buffer[idx]= '\0';
                write(fd,buffer,idx);
                write(fd,"\n",strlen("\n"));
                idx = 0;
            }else {
                buffer[idx++]=c;
            }

        }
    }



}

int main(int argc, char* argv[]){

    file = malloc(sizeof(char *)*argc);
    char **str = malloc(sizeof(char *)*argc);
    int numFile = 0;
    int numStr = 0;
    int mode =  0;

    for (int i = 0; i < argc;i++) {

        if (strcmp(argv[i],"-f")==0) {
            mode = 1;
            continue;
        }else if (strcmp(argv[i],"-s")== 0) {
            mode = 2;
            continue;
        }

        if (mode == 1) {
            file[numFile] = argv[i];
            numFile++;
        }else if (mode == 2) {
            str[numStr] = argv[i];
            numStr++;
        }
    }

    if (numStr != numFile || numStr== 0 || numFile == 0) {
        messaggio("Errore il numero dei file deve essere uguale al numero delle stringhe");
    }

    N = numFile;
    //ignoro il sigint
    signal(SIGINT,SIG_IGN);
    int pipes[N][2];

    for (int i = 0; i<N;i++) {
        if (pipe(pipes[i])==-1) {
            messaggio("errore creazione pipe");
        }
    }


    for (int i = 0 ; i <N; i++) {

        pid_t pid = fork();

        if (pid < 0) messaggio("creazione figlio");
        if (pid == 0) {
            //FIGLIO

            for (int j = 0; j < N; j ++) {
                //chiudo le scritture
                close(pipes[j][1]);
                if (j!= i) {
                    //chiudo tutte le letture tranne la mia
                    close(pipes[j][0]);
                }

            }
            sonWork(pipes[i][0],file[i],str[i]);
        }
    }

//padre
    for (int i = 0; i<N; i++) {
//chiudo tutte le pipe in lettura;
        close(pipes[i][0]);
    }

    char buff[1024];
    while (fgets(buff,sizeof(buff),stdin)!=NULL) {

        buff[strcspn(buff,"\n")]='\0';
        int match = 0;
        for (int i = 0; i <N; i++) {
            if (strcmp(str[i],buff)==0) {

                write(pipes[i][1],buff,strlen(buff)+1);
                match = 1;
                break;
            }
        }
        if (match == 0) {
            for (int i = 0; i <N; i++) {
                write(pipes[i][1],buff,strlen(buff)+1);
            }
        }
    }


    free(file);
    free(str);
	return 0;
}
