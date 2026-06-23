#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

int main (int argc , char *argv[]){

char comando_completo[] = "ls -l /var | date | ps";
char *sottoComandi[10];
 int i = 0;
 char *token= strtok(comando_completo," | ");

 while (token!=NULL)
 {
    sottoComandi[i]=token;
    i++;
    token= strtok(NULL," | ");
}


char *arg[10];
for(int j = 0; j < i ; j++){
    char *sottoToken = strtok(sottoComandi[j]," ");
    int k = 0;
    while (sottoToken!=NULL)
    {
        arg[k] = sottoToken;
        k++;
        sottoToken = strtok(NULL," ");
    }
    arg[k] = NULL;
    pid_t figlio = fork();
    if(figlio == -1) return 0;
    if(figlio ==0){
       if(execvp(arg[0],arg) == -1) return ;
    }else{
        wait(NULL);
    }
    


}

}