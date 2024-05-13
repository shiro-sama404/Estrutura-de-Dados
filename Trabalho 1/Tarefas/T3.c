#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Hash.h"
#include "..\Bibliotecas\NameHash.h"
#include "..\Bibliotecas\KD_Tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

int main(){

    kd_tree * tree = (kd_tree *) malloc(sizeof(kd_tree));
    thash * hash = (thash *) malloc(sizeof(thash));
    tname_hash * name_hash = (tname_hash *) malloc(sizeof(tname_hash));
    
    uint32_t ibge_code;
    short cities_amount;

    if(!InitializeKDTree(tree, hash) || !InitializeNameHash(name_hash, hash)){

        printf("\nEncerrando o programa..\n");
        return EXIT_FAILURE;
    }

    short op = 1;

    while(op){

        printf("\n\n1: Pesquisar cidades mais próximas a partir do nome\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){

            char name[35];

            printf("\nDigite o nome da cidade: ");
            fgets(name, 34, stdin);

            ibge_code = getCityCode(name_hash, name);

            printf("\n%d ", ibge_code);

            if(ibge_code > 0){
                printf("\nDigite a quantidade de cidades: ");
                scanf("%hu", &cities_amount);

                if(cities_amount <= 0){
                    printf("\nInsira no mínimo uma cidade.\n");
                }else if(cities_amount > 5569){
                    printf("\nNão há tantas cidades no Brasil! Insira um valor menor que 5570.\n");
                }else{
                    printf("\nDigite o código ibge da cidade: ");
                    scanf("%d", &ibge_code);

                    tnear * nearest = getNearestCities(tree, hash, ibge_code, cities_amount);
                
                    for(short i = 0; i < cities_amount; i++){
                        displayCityInfo(nearest[i].city);
                        printf("\nDistancia: %f\n\n", nearest[i].distance);
                    }
                        
                }
            }else{
                printf("\nCidade não encontrada.\n");
            }
        }else if(op > 0){
            printf("\nInsira uma opção válida");
        }
    }

    DestroyHash(hash);
    DestroyKDTree(tree);
    DestroyNameHash(name_hash);

    return EXIT_SUCCESS;
}