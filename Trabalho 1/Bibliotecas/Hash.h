/*

*/
#ifndef HASH_H
#define HASH_H

#include "..\Bibliotecas\City.h"
#include "..\Bibliotecas\Jason_Parser.h"

#define SEED 0x12345678

typedef struct{
    tcity ** table;
    uint16_t actualSize;
    uint16_t maxSize;
    tcity * deleted;
}thash;

/*

    Funções básicas da Hash

*/

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
        else
            printf("\nErro ao inserir cidade: Cidade indefinida.\n");
        free(city);
        return false;
    }

    //  Inserindo
    //printf("\nCalculando o hashing de %d..\n", city->ibge_code);

    uint8_t attempt = 0;
    uint32_t hashCode = HashFunction(city->ibge_code, attempt++) % hash->maxSize;

    //printf("\nInserindo cidade de bucket %d..\n", hashCode);

    while(hash->table[hashCode] != NULL && hash->table[hashCode] != hash->deleted)
        hashCode = HashFunction(city->ibge_code, attempt++) % hash->maxSize;

    hash->table[hashCode] = city;
    hash->actualSize++;
    
    //printf("\nCidade inserida com sucesso.\n");
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

    while(hash->table[hashCode] != NULL){

        if (hash->table[hashCode]->ibge_code == ibge_code)
            return hashCode;
        hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;
    }

    return 0;
}

// Busca e retorna uma cidade (se existir) a partir de um código IBGE
tcity * getCity( thash * hash, uint32_t ibge_code ){

    printf("\nCalculando o hashing de %d..\n", ibge_code);

    uint8_t attempt = 0;
    uint16_t hashCode = HashFunction(ibge_code, attempt++) % hash->maxSize;

    while(hash->table[hashCode] != NULL){

        printf("\nProcurando cidade de código %d..\n", hashCode);
        if (hash->table[hashCode]->ibge_code == ibge_code && hash->table[hashCode] != hash->deleted){
            printf("\nCidade encontrada.\n");
            return hash->table[hashCode];
        }
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

    bool existe = true;;

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
        }
        free((tcity *)temp);
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

// Converte um char para o seu respectivo fusohorário
void getTimeZone( char c ){

    if(c == 1)
        printf("America\\/Sao_Paulo\n");
    else if(c == 2)
        printf("America\\/Porto_Velho\n");
    else if(c == 3)
        printf("America\\/Rio_Branco\n");
    else if(c == 4)
        printf("America\\/Noronha\n");
    else
        printf("fuso_horario não encontrado\n");
}

// Imprime todas as informações de uma cidade
void displayCityInfo( tcity * city ){

    if(city){
        printf("\ncodigo_ibge: %d", city->ibge_code);
        printf("\nnome: %s", city->name);
        printf("\nlatitude: %lf", city->latitude);
        printf("\nlongitude: %lf", city->longitude);
        printf("\ncapital: %d", city->capital);
        printf("\ncodigo_uf: %d", city->uf_code);
        printf("\nsiafi_id: %d", city->siafi_id);
        printf("\nddd: %d", city->ddd);
        printf("\nfuso_horario: ");
        getTimeZone(city->time_zone);
    }else{
        printf("\nNULO\n");
    }
}

// Remove uma cidade da hash e marca a posição como deletada
bool RemoveInHash( thash * hash, tcity * city ){

    if(city){

        free(city);
        delete(hash, city);
        printf("\nCidade removida com sucesso.\n");
        return true;
    }else{

        printf("\nCidade não encontrada.\n");
        return false;
    }
}

// Desaloca a tabela hash
void DestroyHash( thash * hash ){

    if(hash){

        printf("\nApagando Tabela Hash..\n\n");
        if(hash->table){

            for (uint16_t position = 0; position < hash->maxSize; position++){

                if (hash->table[position] && hash->table[position] != hash->deleted){

                    //printf("destruindo bucket de número %d\n", position);
                    free((tcity*) (hash->table+position));
                }
            }

            free(hash->table);
            hash->table = NULL;
        }

        free(hash);
        hash = NULL;
        printf("\nTabela Hash Apagada.\n");
    }
}

#endif