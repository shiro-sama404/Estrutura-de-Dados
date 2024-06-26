#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Hash.h"
#include "..\Bibliotecas\KD_Tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

int main(){

    kd_tree * tree = (kd_tree *) malloc(sizeof(kd_tree));
    thash * hash = (thash *) malloc(sizeof(thash));
    
    uint32_t ibge_code;
    short cities_amount;

    if(!InitializeKDTree(tree, hash)){

        printf("\nEncerrando o programa.\n");
        return EXIT_FAILURE;
    }

    uint16_t op = 1;

    while(op){

        printf("\n\n1: Pesquisar cidades mais proximas\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){

            printf("\nDigite a quantidade de cidades: ");
            scanf("%hu", &cities_amount);

            if(cities_amount > 5569){
                printf("\nInsira um valor entre 1 e 5569.\n");
                continue;
            }

            printf("\nDigite o codigo ibge da cidade: ");
            scanf("%d", &ibge_code);

            displayCities(getNearestCities(tree, hash, ibge_code, cities_amount), cities_amount);
            
        }else if(op != 0)
            printf("\nInsira uma opcao valida");
    }

    DestroyHash(hash);
    DestroyKDTree(tree);

    return EXIT_SUCCESS;
}