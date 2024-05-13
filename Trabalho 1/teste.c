#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){


    bool a = false;
    int op = 1;

    do{
        scanf("%d", &op);
        printf("\n%d\n", a);
        a = !a;
    }while(op);

    return EXIT_SUCCESS;
}

//E:/Desk/UF/Estrutura de Dados/Trabalho 1/municipios.txt
//Trabalho 1/municipios.txt