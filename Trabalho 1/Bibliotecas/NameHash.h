/*

*/
#ifndef NAMEHASH_H
#define NAMEHASH_H

#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Hash.h"

#include <stdio.h>
#include <string.h>

typedef struct{
    tcity ** table;
    uint16_t actualSize;
    uint16_t maxSize;
    char * deleted;
}tname_hash;

/*

    Funções básicas da Hash

*/

// Função hash
uint16_t NameHashFunction( const char * key, uint8_t attempt) {
   return (uint16_t)(key + attempt * 61 - ((int)key % 37));
}

// Insere uma cidade na tabela hash
void InsertInNameHash( tname_hash * name_hash, tcity * city ){

    //  Validando a inserção
    if(name_hash->maxSize == (name_hash->actualSize++))
        printf("\nErro ao inserir nome: Tabela cheia.\n");
    else if(city == NULL)
        printf("\nErro ao inserir nome: nome indefinido.\n");
    else{
        //  Inserindo
        uint8_t attempt = 0;
        uint16_t hashCode = NameHashFunction(city->name, attempt++) % name_hash->maxSize;

        while(name_hash->table[hashCode])
            hashCode = NameHashFunction(city->name, attempt++) % name_hash->maxSize;

        name_hash->table[hashCode] = city;
        name_hash->actualSize++;
    }
}

// Inicializa a tabela name hash com os nomes obtidos da tabela hash geral
bool InitializeNameHash( tname_hash * name_hash, thash * hash ){

    //  Inicialização padrão da hash
    printf("\nInicializando a name hash..\n");

    name_hash->table = (char **) calloc(11131, sizeof(char *));

    if (name_hash->table == NULL)
        return false;
    
    name_hash->actualSize = 0;
    name_hash->maxSize = 11131;

    for(short i = 0; i < name_hash->maxSize; i++)
        if(hash->table[i])
            InsertInNameHash(name_hash, hash->table[i]);

    return true;
}

// Retorna o código IBGE a partir do nome da cidade
uint32_t getCityCode( tname_hash * hash, char * name ){
    
    printf("\nCalculando o hashing de %s..\n", name);

    uint8_t attempt = 0;
    uint16_t hashCode = NameHashFunction(name, attempt++) % hash->maxSize;

    tcity ** options = (tcity **) malloc(sizeof(tcity *));
    uint8_t actual_size = 0;

    while(hash->table[hashCode]){

        if (strcmp(hash->table[hashCode]->name, name) == 0){
            printf("\nEntrou");
            options[actual_size] = hash->table[hashCode];
            actual_size = actual_size + 1;
        }
        hashCode = NameHashFunction(name, attempt++) % hash->maxSize;
    }

    short op = 0;

    if(actual_size == 0){
        return 0;
    }

    if(actual_size > 1){
        printf("\nHá mais de uma cidade com esse nome. Escolha uma:\n");
        for(short i = 0; i < actual_size; i++){
            printf("Código IBGE: %d\n", options[i]->ibge_code);
        }
        scanf("%hu", &op);
    }

    return options[op]->ibge_code;
}

// Desaloca a tabela name hash
void DestroyNameHash( tname_hash * hash ){

    if(hash){

        printf("\nApagando Tabela Name Hash..\n\n");

        if(hash->table){

            for (uint16_t position = 0; position < hash->maxSize; position++)
                if ((hash->table+position))
                    free((tcity*) (hash->table+position));

            free(hash->table);
            hash->table = NULL;
        }

        free(hash);
        hash = NULL;
        printf("\nTabela Name Hash Apagada.\n");
    }
}

#endif