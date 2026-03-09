//
// Created by davidjulianbelfiori on 04/03/26.
//
#include <stdio.h>

int *x;
double *y;
void function(void){
    x = y;
    if ( x+1 == y+1 ) return 1; // Tale predicato non è mai verificato!!!
    return 0;
}