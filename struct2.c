#include <stdio.h>


#define  fflush(stdin) while(getchar() != '\n');
int main(int argc, char *argv[]){

    typedef struct table_entry{
        int x;
        float y;
    }table_entry;


    table_entry t;

   // scanf("%d %f",&t.x,&t.y);
  //   printf("x: %d ; y: %f",t.x,t.y);

    table_entry db[5];

    for (int i = 0; i < 5; i++)
    {
        while (1){
           printf("db %d-esimo intero e float: ",i);
        if(scanf("%d %f",&db[i].x,&db[i].y) == 2){
           break;
        }else{
            printf("si prega di riprovare \n");
            fflush(stdin);
        };
        }
        
        
    }
    for (int i = 0; i < 5; i++)
    {
        printf("db %d-esimo intero %d e float %f \n",i,db[i].x,db[i].y);
        
    }
    

    return 0;
}

