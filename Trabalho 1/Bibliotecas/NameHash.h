/*

*/
#ifndef NAMEHASH_H
#define NAMEHASH_H

#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Hash.h"

#include <stdio.h>
#include <string.h>

/*               Funções básicas da Hash para nomes                  */

// Função hash
uint16_t NameHashFunction( const char * key, uint16_t len, uint8_t attempt) {
    return (uint16_t)(*(key + (attempt % len)) * 61 - ((*(key + ((7 * attempt) % len))) * attempt));
}

// Insere uma cidade na tabela hash
void InsertInNameHash( thash * name_hash, tcity * city ){

    //  Validando a inserção
    if(name_hash->maxSize == (name_hash->actualSize++))
        printf("\nErro ao inserir nome: Tabela cheia.\n");
    else if(city == NULL)
        printf("\nErro ao inserir nome: nome indefinido.\n");
    else{
        //  Inserindo
        uint8_t attempt = 0;
        uint16_t length = (uint16_t) strlen(city->name) - 1;
        uint16_t hashCode = NameHashFunction(city->name, length, attempt++) % name_hash->maxSize;
        
        while(name_hash->table[hashCode])
        hashCode = NameHashFunction(city->name, length, attempt++) % name_hash->maxSize;

        name_hash->table[hashCode] = city;
        name_hash->actualSize++;
    }
}

// Inicializa a tabela name_hash com os dados obtidos da tabela hash geral
bool InitializeNameHash( thash * name_hash, thash * hash ){

    //  Inicialização padrão da hash
    name_hash->table = (tcity **) calloc(11131, sizeof(tcity *));

    if (name_hash->table == NULL)
        return false;
    
    name_hash->actualSize = 0;
    name_hash->maxSize = 11131;

    for(int i = 0; i < name_hash->maxSize; i++)
        if(hash->table[i])
            InsertInNameHash(name_hash, hash->table[i]);

    return true;
}

// Retorna o código IBGE a partir do nome da cidade
uint32_t getCityCodeByName( thash * name_hash, char * name ){

    // Buscando todas as cidades com o mesmo nome
    tcity ** options = (tcity **) malloc(10 * sizeof(tcity *)); // Supondo que para cada nome haja no máximo 10 cidades
    uint8_t options_amount = 0;

    uint8_t attempt = 0;
    uint16_t length = (uint16_t) strlen(name) - 1;
    uint16_t hashCode = NameHashFunction(name, length, attempt++) % name_hash->maxSize;

    while(name_hash->table[hashCode] || name_hash->table[hashCode] == name_hash->deleted){

        if (strcmp(name_hash->table[hashCode]->name, name) == 0){
            options[options_amount] = name_hash->table[hashCode];
            options_amount = options_amount + 1;
        }
        hashCode = NameHashFunction(name, length, attempt++) % name_hash->maxSize;
    }

    short op = 0;

    if(options_amount == 0)
        return 0;

    if(options_amount > 1){

        printf("\nHa mais de uma cidade com esse nome. Escolha uma:\n");

        for(short i = 0; i < options_amount; i++)
            printf("opcao %d: Codigo IBGE: %d\n", i + 1, options[i]->ibge_code);

        scanf("%hu", &op);
    }

    return options[op-1]->ibge_code;
}

#endif