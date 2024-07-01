#ifndef CITY_H
#define CITY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//  Struct cidade
typedef struct{
    uint32_t ibge_code;
    char * name;
    double latitude;
    double longitude;
    bool capital;
    uint8_t uf_code;
    uint16_t siafi_id;
    uint8_t ddd;
    char time_zone;           //Apenas 4 opções (Sao Paulo, Porto Velho, Rio Branco e Noronha)
}tcity;

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
    }
}

// Retorna a distância entre 2 cidades
double getCityDistance( tcity * a_city, tcity * b_city ){

    // Aplicando a fórmula da distância entre 2 pontos sem raíz
    double distance = fabs(a_city->latitude - b_city->latitude) + fabs(a_city->longitude - b_city->longitude);
    return distance;
}

// Retorna a distância entre 2 cidades
double getCityDistanceSQRT( tcity * a_city, tcity * b_city ){

    // Aplicando a fórmula da distância entre 2 pontos sem raíz
    double distance = sqrt(pow((a_city->latitude - b_city->latitude), 2) + pow((a_city->longitude - b_city->longitude), 2));
    return distance;
}

#endif