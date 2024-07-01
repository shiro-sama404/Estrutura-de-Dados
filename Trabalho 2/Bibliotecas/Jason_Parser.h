/*
    
    Jason Parser

    Responsável pela leitura, extração e conversão de dados do municipios.json
*/
#ifndef JASONPARSER_H
#define JASONPARSER_H

#include "..\Bibliotecas\City.h"
#include <stdio.h>
#include <string.h>

// Converte um dos 4 fusohorários possíveis para um char
char SetTimeZoneCode(char * timeZone){

    if(strcmp(timeZone, "America\\/Sao_Paulo") == 0)
        return 1;
    else if(strcmp(timeZone, "America\\/Porto_Velho") == 0)
        return 2;
    else if(strcmp(timeZone, "America\\/Rio_Branco") == 0)
        return 3;
    else if(strcmp(timeZone, "America\\/Noronha") == 0)
        return 4;
    else
        return 0;
}

// Desloca caractetes de uma string para esquerda (retira " lida no início de um atributo String)
void shiftLeft(char * str, uint8_t pos){

    while(str[pos] != '\0'){

        str[pos] = str[pos+1];
        pos++;
    }
}

// Formata String obtida na leitura de atributos para ser convertida
void formatString(char * str){

    uint8_t len = strlen(str)-1;
    str[len] = '\0';
    len--;

    if(str[len] == '\"'){
        str[len] = str[len+1];
        shiftLeft(str, 0);
    }
}

// Lê, formata e retorna o próximo atributo do arquivo municipios.json
char * GetNextJsonAtribute(FILE * jsonFile, char * actualChar, char * contentBuffer){
    
    while(*actualChar != ':' && *actualChar != EOF)
        *actualChar = fgetc(jsonFile);

    if(*actualChar == EOF)
        return "0";

    *actualChar = fgetc(jsonFile);

    memset(contentBuffer, '\0', 50);

    uint8_t content_size = 0;

    while(*actualChar != ',' && *actualChar != '\n'){

        contentBuffer[content_size] = fgetc(jsonFile);
        *actualChar = contentBuffer[content_size];
        content_size++;
    }

    if(contentBuffer[content_size-2] == '\"' || contentBuffer[content_size-2] == '}'){
        
        char * content = (char *) malloc(sizeof(char)*content_size);

        formatString(contentBuffer);
        strcpy(content, contentBuffer);

        return content;
    }else{

        formatString(contentBuffer);
        return contentBuffer;
    }
    
}

// Acessa o arquivo JSON e retorna um ponteiro do tipo tcity alocado e inicializado com a próxima cidade do arquivo
tcity * GetCityInParser(FILE * jsonFile, char * actualChar, char * contentBuffer){

    tcity * new = (tcity *) malloc(sizeof(tcity));

    if(new == NULL){
        return NULL;
    }

    // Extraindo os dados do arquivo JSON
    new->ibge_code = atoi(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));
    new->name      =      GetNextJsonAtribute(jsonFile, actualChar, contentBuffer);

    sscanf(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer), "%lf", &new->latitude);
    sscanf(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer), "%lf", &new->longitude);

    new->capital   = atoi(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));
    new->uf_code   = atoi(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));
    new->siafi_id  = atoi(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));
    new->ddd       = atoi(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));
    new->time_zone = SetTimeZoneCode(GetNextJsonAtribute(jsonFile, actualChar, contentBuffer));

    if(*actualChar == EOF){
        return NULL;
    }
    
    return new;
}

#endif