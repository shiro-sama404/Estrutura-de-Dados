#include "..\Bibliotecas\Hash.h"
#include "..\Bibliotecas\AVL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#define BiggestName 32 // Cidade com maior nome do Brasil: 32 caracteres (Vila Bela da Santíssima Trindade)

void destroyALL(thash * hash, tAVL (*AVL_trees)[5]){

    DestroyHash(hash);

    printf("\nApagando AVL..");
    for(short i = 0; i < 5; i++)
        destroyRoot((*AVL_trees[i]).root);
}

int main(){

    thash * hash = (thash *) malloc(sizeof(thash));

    tAVL AVL_trees[5];
    tAVL name_AVL;
    tAVL latitude_AVL;
    tAVL longitude_AVL;
    tAVL codigo_uf_AVL;
    tAVL ddd_AVL;

    name_AVL.root = NULL;
    latitude_AVL.root = NULL;
    longitude_AVL.root = NULL;
    codigo_uf_AVL.root = NULL;
    ddd_AVL.root = NULL;

    AVL_trees[0] = name_AVL;
    AVL_trees[1] = latitude_AVL;
    AVL_trees[2] = longitude_AVL;
    AVL_trees[3] = codigo_uf_AVL;
    AVL_trees[4] = ddd_AVL;

    if(!InitializeHash(hash)){

        printf("\nErro ao inicializar hash. Encerrando o programa..\n");
        return EXIT_FAILURE;
    }

    for(short avl_type = 0; avl_type < 5; avl_type++){
        if(!InitializeAVL(&AVL_trees[avl_type], hash, avl_type)){
            printf("\nErro ao inicializar AVL. Encerrando o programa..\n");
            return EXIT_FAILURE;
        }
    }

    uint16_t op = 1;
    tquery * query_result; // Lista encadeada com os códigos IBGE que atendem a range query

    while(op){

        query_result = NULL;

        printf("\n\n1: Range Query\n0: Sair\n\n");
        scanf("%hu", &op);
            
        if(op == 1){


            printf("\nOpcoes para a query:\n");
            char options[5][12] = { "nome", "latitude", "longitude", "codigo de UF", "DDD" };

            for(short avl_type = 0; avl_type < 5; avl_type++){

                printf("\nDeseja adicionar um intervalo de %s para a busca?\n1: sim\n", options[avl_type]);

                scanf("%hu", &op);

                if(op == 1){

                    // Faz o query na AVL de nomes (AVL_trees[0])
                    if(avl_type == 0){
                        
                        char min_range[BiggestName];
                        printf("\nLimite inferior: ");
                        getchar();
                        fgets(min_range, BiggestName, stdin);
                        min_range[strlen(min_range)-1] = min_range[strlen(min_range)];

                        char max_range[BiggestName];
                        printf("\nLimite superior: ");
                        getchar();
                        fgets(max_range, BiggestName, stdin);
                        max_range[strlen(max_range)-1] = max_range[strlen(max_range)];

                        if(strcmp(min_range, max_range) >= 0){
                            printf("\nO limite superior deve ser maior que o inferior");
                            continue;
                        }

                        Query(query_result, AVL_trees[avl_type].root, (void *)min_range, (void *)max_range, avl_type);

                    // Faz o query na AVL de floats (AVL_trees[1] e (AVL_trees[2]))
                    }else if(avl_type < 3){

                        double min_range , max_range;
                        double * pmin_range = &min_range , * pmax_range = &max_range;
                
                        printf("\nLimite inferior: ");
                        scanf("%lf", pmin_range);
                        printf("\nLimite superior: ");
                        scanf("%lf", pmax_range);

                        if(min_range > max_range){
                            printf("\nO limite superior deve ser maior que o inferior");
                            continue;
                        }

                        Query(query_result, AVL_trees[avl_type].root, (void *)pmin_range, (void *)pmax_range, avl_type);
                    
                    // Faz o query na AVL de inteiros (AVL_trees[3] e (AVL_trees[4]))
                    }else{

                        int aux_min_range, aux_max_range;

                
                        printf("\nLimite inferior: ");
                        scanf("%d", &aux_min_range);
                        printf("\nLimite superior: ");
                        scanf("%d", &aux_max_range);

                        uint16_t min_range = (uint8_t)aux_max_range, max_range = (uint8_t)aux_max_range;
                        uint16_t * pmin_range = &min_range, * pmax_range = &max_range;

                        if(min_range > max_range){
                            printf("\nO limite superior deve ser maior que o inferior");
                            continue;
                        }

                        Query(query_result, AVL_trees[avl_type].root, (void *)pmin_range, (void *)pmax_range, avl_type);
                    }
                }
            }

            printf("\nResultado:\n");

            // Mostra as informações das cidades que satisfazem a range query
            if(query_result){

                tquery * actual = query_result;
                tquery * next = query_result->next;

                while(query_result){
                    displayCityInfo(getCity(hash, actual->ibge_code));
                    free(actual);
                    actual = next;
                    next = next->next;
                }
            }else
                printf("\nNenhuma cidade satisfez as condicoes de busca\n");

        }else if(op > 0)
            printf("\nInsira uma opcao valida");
    }

    destroyALL(hash, &(AVL_trees));

    return EXIT_SUCCESS;
}