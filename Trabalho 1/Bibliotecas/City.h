#ifndef CITY_H
#define CITY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

#endif