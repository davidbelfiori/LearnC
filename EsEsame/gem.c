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

// Variabile globale per il flag del segnale nei processi figli
volatile sig_atomic_t backup_flag = 0;

void sigint_handler(int sig) {
    backup_flag = 1;
}

// Funzione helper per il backup del file
void esegui_backup(const char* filename) {
    char backup_name[512];
    snprintf(backup_name, sizeof(backup_name), "%s_backup", filename);

    int fd_in = open(filename, O_RDONLY);
    if (fd_in < 0) return;

    int fd_out = open(backup_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        close(fd_in);
        return;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd_in, buffer, sizeof(buffer))) > 0) {
        // Scrittura del blocco letto sul file di backup
        write(fd_out, buffer, bytes_read);
    }

    close(fd_in);
    close(fd_out);
}

int main(int argc, char** argv){
    char **files = malloc(argc * sizeof(char*));
    char **strings = malloc(argc * sizeof(char*));
    int n_files = 0;
    int n_strings = 0;

    // 1. Parsing della linea di comando
    int mode = 0; // 1 = parsing file, 2 = parsing stringhe
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-f") == 0) {
            mode = 1;
            continue;
        } else if(strcmp(argv[i], "-s") == 0) {
            mode = 2;
            continue;
        }

        if(mode == 1) {
            files[n_files++] = argv[i];
        } else if(mode == 2) {
            strings[n_strings++] = argv[i];
        }
    }

    if(n_files == 0 || n_files != n_strings) {
        fprintf(stderr, "Errore: Il numero dei file (%d) deve essere uguale al numero delle stringhe (%d) ed N > 0.\n", n_files, n_strings);
        fprintf(stderr, "Uso: %s -f file1 [file2]... -s stringa1 [stringa2]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = n_files;
    int pipes[N][2];

    // 2. Il processo principale ignora SIGINT
    signal(SIGINT, SIG_IGN);

    // 3. Creazione pipes e processi figli
    for(int i = 0; i < N; i++) {
        if(pipe(pipes[i]) == -1) {
            perror("Errore creazione pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if(pid == -1) {
            perror("Errore fork");
            exit(EXIT_FAILURE);
        }

        if(pid == 0) {
            // --- CODICE PROCESSO FIGLIO i-esimo ---

            // Chiusura di tutti i descrittori non necessari
            for(int j = 0; j <= i; j++) {
                close(pipes[j][1]); // Chiude gli estremi di scrittura
                if(j < i) close(pipes[j][0]); // Chiude in lettura le pipe precedenti
            }

            int pipe_lettura = pipes[i][0];
            const char* mio_file = files[i];

            // Impostazione handler per SIGINT
            struct sigaction sa;
            memset(&sa, 0, sizeof(sa));
            sa.sa_handler = sigint_handler;
            sigaction(SIGINT, &sa, NULL);

            // Apertura/Rigenerazione file (tronca a 0 byte allo startup)
            int fd_out = open(mio_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd_out < 0) {
                perror("Errore apertura file nel figlio");
                exit(EXIT_FAILURE);
            }

            char buffer[MAX_LINE];
            int idx = 0;

            // Ciclo principale del figlio (CPU idle in caso di assenza di I/O)
            while(1) {
                // Controllo se è stato ricevuto un segnale SIGINT
                if(backup_flag) {
                    esegui_backup(mio_file);
                    backup_flag = 0;
                }

                char c;
                ssize_t res = read(pipe_lettura, &c, 1);

                if(res == -1) {
                    if(errno == EINTR) continue; // Lettura interrotta da segnale, riprova
                    break; // Altro errore
                }
                if(res == 0) break; // Il padre ha chiuso la pipe (EOF)

                if(c == '\0') {
                    // Stringa terminata: scrivi su file seguita da '\n'
                    buffer[idx] = '\0';
                    write(fd_out, buffer, idx);
                    write(fd_out, "\n", 1);
                    idx = 0; // Resetta per il prossimo messaggio
                } else {
                    if(idx < MAX_LINE - 1) {
                        buffer[idx++] = c;
                    }
                }
            }

            close(pipe_lettura);
            close(fd_out);
            exit(EXIT_SUCCESS);
        }
    }

    // --- CODICE PROCESSO PADRE ---

    // Chiude tutti gli estremi di lettura
    for(int i = 0; i < N; i++) {
        close(pipes[i][0]);
    }

    char input_buf[MAX_LINE];

    // Lettura indefinita da stdin
    while(fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        // Rimuove il newline inserito da fgets
        input_buf[strcspn(input_buf, "\n")] = '\0';

        int match_found = 0;

        // Cerca una corrispondenza
        for(int i = 0; i < N; i++) {
            if(strcmp(input_buf, strings[i]) == 0) {
                // Trovato match con l'i-esima stringa, comunica all'i-esimo figlio
                write(pipes[i][1], input_buf, strlen(input_buf) + 1);
                match_found = 1;
                break; // Se ne troviamo una, non continuiamo a cercare
            }
        }

        // Se non uguale ad alcuna stringa, manda a tutti i figli
        if(!match_found) {
            for(int i = 0; i < N; i++) {
                write(pipes[i][1], input_buf, strlen(input_buf) + 1);
            }
        }
    }

    // In caso di EOF (es. CTRL+D), chiude le pipe per far terminare i figli in sicurezza
    for(int i = 0; i < N; i++) {
        close(pipes[i][1]);
    }

    // Attende la terminazione dei figli (opzionale ma consigliato per evitare zombie)
    for(int i = 0; i < N; i++) {
        wait(NULL);
    }

    free(files);
    free(strings);
    return 0;
}//
// Created by davidjulianbelfiori on 04/07/26.
//
