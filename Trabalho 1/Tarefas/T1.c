//#include "..\\Bibliotecas\Trabalho.h"
#include "..\Bibliotecas\Hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

int main(){

    thash * hash = (thash*) malloc(sizeof(thash));
    uint32_t ibge_code;

    if(!InitializeHash(hash)){

        printf("\nEncerrando o programa.\n");
        return EXIT_FAILURE;
    }

    short op = 1;

    while(op){

        printf("\n1: Pesquisar Cidade\n2: Testar hash\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){

            printf("\nDigite o código ibge da cidade: ");
            scanf("%d", &ibge_code);

            displayCityInfo(getCity(hash, ibge_code));
        }else if(op == 2){

            testHash(hash);
        }else
            printf("\nInsira uma opção válida");
    }

    DestroyHash(hash);

    return EXIT_SUCCESS;
}