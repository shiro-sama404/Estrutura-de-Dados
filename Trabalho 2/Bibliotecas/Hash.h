#ifndef HASH_H
#define HASH_H

#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Jason_Parser.h"

typedef struct{
    tcity ** table;
    uint16_t actualSize;
    uint16_t maxSize;
    tcity * deleted;
}thash;

/*               Funções básicas da Hash                  */

// Função hash
uint16_t HashFunction( const uint32_t key, uint8_t attempt) {
   return (uint16_t)(key + attempt * 61 - (key % 37));
}

// Define o deleted da tabela hash
bool defineDeleted( tcity * city ){

    city = (tcity *) malloc(sizeof(tcity));

    if(city){

        city->ibge_code = 0;
        city->name      = "";
        city->latitude  = 0;
        city->longitude = 0;
        city->capital   = 0;
        city->uf_code   = 0;
        city->siafi_id  = 0;
        city->ddd       = 0;
        city->time_zone = '0';
        return true;
    }
    return false;
}

// Verifica se uma cidade da tabela hash foi deletada
bool isDeleted( thash * hash, tcity * city ){

    if(city == hash->deleted)
        return true;
    return false;
}

// Deleta uma cidade da tabela hash
void delete( thash * hash, tcity * city ){

    free(city);
    city = hash->deleted;
}

// Insere uma cidade na tabela hash
bool InsertInHash( thash * hash, tcity * city ){

    //  Validando a inserção
    if (hash->maxSize == (hash->actualSize++) || city == NULL){

        if(hash->maxSize == (hash->actualSize++))
            printf("\nErro ao inserir cidade: Tabela cheia.\n");
            
        free(city);
        return false;
    }

    //  Inserindo
    uint8_t attempt = 0;
    uint32_t hashCode = HashFunction(city->ibge_code, attempt++) % hash->maxSize;

    while(hash->table[hashCode] && hash->table[hashCode] != hash->deleted)
        hashCode = HashFunction(city->ibge_code, attempt++) % hash->maxSize;

    hash->table[hashCode] = city;
    hash->actualSize++;
    
    return true;
}

// Inicializa a tabela hash com os dados do municipios.json
bool InitializeHash( thash * hash ){

    //  Inicialização padrão da hash
    printf("\nInicializando a hash..\n");

    hash->table = (tcity **) calloc(11131, sizeof(tcity *));

    if (hash->table == NULL || !defineDeleted(hash->deleted))
        return false;
    
    hash->actualSize = 0;
    hash->maxSize = 11131;

    //  Leitura do arquivo json e gravação dos dados na hash
    printf("\nAbrindo arquivo json..\n");
    FILE *jsonFile = fopen("../../src/municipios.json", "r");

    if (jsonFile){
        
        printf("\nInserindo cidades na hash..\n");

        char * contentBuffer = (char *) malloc(sizeof(char)*50);
        char * actualChar = (char *) malloc(sizeof(char));

        * actualChar = (char) fgetc(jsonFile);
        
        while(InsertInHash(hash, GetCityInParser(jsonFile, actualChar, contentBuffer))){}

        printf("\nSucesso no Parser.\n");

        // Liberando os ponteiros para leitura do arquivo json
        free(actualChar);
        free(contentBuffer);
        free(jsonFile);
    }else{
        printf("\nErro: não foi possível abrir o arquivo.\n");
        return false;
    }

    return true;
}

// Retorna a posição de uma cidade na tabela hash
uint16_t getCityIndex( thash * hash, uint32_t ibge_code ){
    
    printf("\nCalculando o hashing de %d..\n", ibge_code);

    uint8_t attempt = 0;
    uint16_t hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;

    while(hash->table[hashCode]){

        if (hash->table[hashCode]->ibge_code == ibge_code)
            return hashCode;
        hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;
    }

    return 0;
}

// Busca e retorna uma cidade (se existir) a partir de um código IBGE
tcity * getCity( thash * hash, uint32_t ibge_code ){

    uint8_t attempt = 0;
    uint16_t hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;

    while(hash->table[hashCode]){

        if (hash->table[hashCode]->ibge_code == ibge_code && hash->table[hashCode] != hash->deleted)
            return hash->table[hashCode];
        hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;
    }

    printf("\nCidade não encontrada.\n");
    return NULL;
}

// Testa se todos as cidades foram inseridas na hash
void testHash( thash * hash ){

    printf("\n\nTestando hash\n");

    FILE *jsonFile = fopen("../../src/municipios.json", "r");
    char * contentBuffer = (char *) malloc(sizeof(char)*50);
    char * actualChar = (char *) malloc(sizeof(char));

    bool existe = true;

    * actualChar = (char) fgetc(jsonFile);

    tcity * temp;
    uint16_t amount = 0;

    printf("\nVerificando se todas as cidades foram devidamente inseridas..\n");

    while(existe && * actualChar != EOF){

        temp = GetCityInParser(jsonFile, actualChar, contentBuffer);

        if(temp){
            
            if(temp->ibge_code != getCity(hash, temp->ibge_code)->ibge_code)
                existe = false;
            amount = amount + 1;

            free((tcity *)temp);
        }

        printf("\nDepois\n");
    }

    if(existe)
        printf("\nTodas as cidades foram devidamente inseridas.\nTotal de cidades: %hu\n", amount);
    else
        printf("\nErro: há cidades que não foram inseridas.\n");

    // Liberando os ponteiros para leitura do arquivo json
    free(actualChar);
    free(contentBuffer);
    free(jsonFile);
}

// Remove uma cidade da hash e marca a posição como deletada
bool RemoveInHash( thash * hash, tcity * city ){

    if(city){

        free(city);
        delete(hash, city);
        printf("\nCidade removida com sucesso.\n");
        return true;
    }

    printf("\nCidade não encontrada.\n");
    return false;
}

// Desaloca a tabela hash
void DestroyHash( thash * hash ){

    if(hash){

        printf("\nApagando Tabela Hash..");

        if(hash->table){

            for (uint16_t position = 0; position < hash->maxSize; position++)
                if (hash->table[position] && hash->table[position] != hash->deleted)
                    free((tcity*) (hash->table+position));

            free(hash->table);
            hash->table = NULL;
        }

        free(hash);
        hash = NULL;
        printf("\nTabela Hash Apagada.\n");
    }
}

#endif