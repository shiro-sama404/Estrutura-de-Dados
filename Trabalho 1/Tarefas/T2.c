#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Hash.h"
#include "..\Bibliotecas\AVL_Tree.h"
#include "..\Bibliotecas\KD_Tree.h"
#include "..\Bibliotecas\Trabalho.h"

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

    short op = 1;

    while(op){

        printf("\n\n1: Pesquisar cidades mais próximas\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){

            printf("\nDigite a quantidade de cidades: ");
            scanf("%hu", &cities_amount);

            if(cities_amount <= 0){
                printf("\nInsira no mínimo uma cidade.\n");
            }else if(cities_amount > 5569){
                printf("\nNão há tantas cidades no Brasil! Insira um valor menor que 5570.\n");
            }else{
                printf("\nDigite o código ibge da cidade: ");
                scanf("%d", &ibge_code);

                displayCities(getNearestCities(tree, hash, ibge_code, cities_amount), cities_amount);
            }

        }else if(op > 0){
            //int qtd = contaNodes2(tree->root);
            printf("\nInsira uma opção válida");
            //printf("\nNodes na árvore: %d", qtd);
        }
    }

    DestroyHash(hash);
    DestroyKDTree(tree);

    return EXIT_SUCCESS;
}