//
// Created by davidjulianbelfiori on 04/03/26.
//
#include <stdio.h>

int main(int x) {
 if ( x == 1 ) return 1;
    return 0;

}
/* 400446:	55                   	push   %rbp
400447:	48 89 e5             	mov    %rsp,%rbp Imposta il nuovo Base Pointer all'attuale posizione dello Stack Pointer. Da qui in poi, le variabili locali saranno riferite a %rbp.
40044a:	89 7d fc             	mov    %edi,-0x4(%rbp)
In x86_64, il primo argomento intero di una funzione (int x) viene passato nel registro %edi.
Qui il compilatore lo sposta nello stack (4 byte sotto %rbp) per poterlo usare nel confronto.
40044d:	83 7d fc 01          	cmpl   $0x1,-0x4(%rbp)
400451:	75 07                	jne    40045a <main+0x14> jump not equal
400453:	b8 01 00 00 00       	mov    $0x1,%eax
400458:	eb 05                	jmp    40045f <main+0x19>
40045a:	b8 00 00 00 00       	mov    $0x0,%eax per convezione un valore di ritorno va inserito in %eax
40045f:	5d                   	pop    %rbp Ripristina il Base Pointer originale del chiamante.
400460:	c3                      ret
*/