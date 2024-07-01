#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\NameHash.h"
#include "..\Bibliotecas\KD_Tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define BiggestName 32 // Cidade com maior nome do Brasil: 32 caracteres (Vila Bela da Santíssima Trindade)

int main(){

    kd_tree * tree = (kd_tree *) malloc(sizeof(kd_tree));
    thash * hash = (thash *) malloc(sizeof(thash));
    thash * name_hash = (thash *) malloc(sizeof(thash));
    
    uint32_t ibge_code;
    uint16_t cities_amount;

    if(!InitializeKDTree(tree, hash) || !InitializeNameHash(name_hash, hash)){

        printf("\nEncerrando o programa..\n");
        return EXIT_FAILURE;
    }

    uint16_t op = 1;

    while(op){

        printf("\n\n1: Pesquisar cidades mais proximas a partir do nome\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){

            char name[BiggestName + 1];

            printf("\nDigite o nome da cidade: ");
            getchar();
            fgets(name, BiggestName, stdin);

            name[strlen(name)-1] = name[strlen(name)];

            ibge_code = getCityCodeByName(name_hash, name);

            printf("\n%d ", ibge_code);

            if(ibge_code > 0){
                printf("\nDigite a quantidade de cidades: ");
                scanf("%hu", &cities_amount);

                if(cities_amount > 5569){
                    printf("\nInsira um valor entre 1 e 5569.\n");
                    continue;
                }

                tnear * nearest = getNearestCities(tree, hash, ibge_code, cities_amount);
            
                for(short i = 0; i < cities_amount; i++){
                    displayCityInfo(nearest[i].city);
                    printf("distancia: %f\n\n", nearest[i].distance);
                }
                
            }else
                printf("\nCidade nao encontrada.\n");

        }else if(op > 0)
            printf("\nInsira uma opcao valida");
    }

    DestroyHash(hash);
    DestroyHash(name_hash);
    DestroyKDTree(tree);

    return EXIT_SUCCESS;
}