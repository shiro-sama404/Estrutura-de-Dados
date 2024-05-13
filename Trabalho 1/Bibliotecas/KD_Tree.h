/*

    Árvore KD

    

*/
#ifndef KDTREE_H
#define KDTREE_H

#include "..\Bibliotecas\Hash.h"

#include <math.h>

typedef struct _kd_node{
    tcity * city;
    uint16_t height;
    struct _kd_node * left;
    struct _kd_node * right;
}kd_node;

typedef struct{
    kd_node * root;
}kd_tree;

typedef struct _nearest{
    tcity * city;
    double distance;
}tnear;

/*

    Funções básicas da árvore KD

*/

// Retorna a distância entre 2 cidades
double getCityDistance( tcity * a_city, tcity * b_city ){

    // Aplicando a fórmula da distância entre 2 pontos sem raíz
    double distance = pow((a_city->latitude - b_city->latitude), 2) +pow((a_city->longitude - b_city->longitude), 2);
    return distance;
}

// Aloca um node para árvore kd e retorna o sucesso ou fracasso da alocação
bool newKDNode(kd_node * new, tcity * city){

    if(new){

        new->city = city;
        new->height = 0;
        new->left = NULL;
        new->right = NULL;

        return true;
    }
    return false;
}

// Inserção na árvore KD
bool InsertInKDTree( kd_tree * tree, tcity * city ){

    if(city == NULL)
        return false;

    kd_node * new = (kd_node *) malloc(sizeof(kd_node));

    if(!newKDNode(new, city)){

        printf("\nNão foi possível alocar novo node\n");
        return false;
    }

    //printf("\nInserindo cidade de código %d na árvore", new->city->ibge_code);

    if(tree->root == NULL)
        tree->root = new;
    else{

        kd_node * temp = tree->root;
        bool axis = 0;

        while(temp != NULL){

            if(axis == 0 ? (new->city->latitude > temp->city->latitude) : (new->city->longitude > temp->city->longitude)){
                if(temp->right)
                    temp = temp->right;
                else{
                    temp->right = new;
                    break;
                }
            }else{
                if(temp->left)
                    temp = temp->left;
                else{
                    temp->left = new;
                    break;
                }
            }

            axis = !axis;
        }
    }

    return true;
}

// Inicialização da árvore KD e da hash com os dados do municipios.json
bool InitializeKDTree( kd_tree * tree, thash * hash ){

    tree->root = NULL;

    //  Alocando hash
    printf("\nAlocando hash..\n");
    hash->table = (tcity **) calloc(11131, sizeof(tcity *));

    if (hash->table == NULL || !defineDeleted(hash->deleted))
        return false;
    
    hash->actualSize = 0;
    hash->maxSize = 11131;

    //  Alocando hash
    printf("\nInicializando a árvore e a hash ..\n");

    //  Leitura do arquivo json e inserção dos dados na árvore
    printf("\nAbrindo arquivo json..\n");
    FILE *jsonFile = fopen("../../src/municipios.json", "r");

    if (jsonFile){
        
        printf("\nInserindo cidades na árvore..\n");

        char * contentBuffer = (char *) malloc(sizeof(char)*50);
        char * actualChar = (char *) malloc(sizeof(char));

        * actualChar = (char) fgetc(jsonFile);

        tcity * temp = GetCityInParser(jsonFile, actualChar, contentBuffer);

        while(InsertInKDTree(tree, temp) && InsertInHash(hash, temp))
            temp = GetCityInParser(jsonFile, actualChar, contentBuffer);
        
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

// Destrói todos os nodes a partir de um node (deve ser chamado a partir da raíz)
void DestroyNodes(kd_node * node){

    if(node){
        DestroyNodes(node->left);
        DestroyNodes(node->right);
        free((kd_node *) node);
    }
    
}

// Destrói a árvore KD e todos os seus nodes
void DestroyKDTree( kd_tree * tree ){

    printf("\nDestruindo nós..\n");

    DestroyNodes(tree->root);
    free((kd_tree *) tree);

    printf("\nÁrvore destruida com sucesso.\n");
}

// Verifica se a melhor cidade encontrada já está na árvore ou é a própria cidade procurada
bool isNotRestricted( tnear * nearest, kd_node * node, tcity * target_city, short * size ){

    //printf("\n\nEntrou isNotRestricted 1\n\n");

    if(nearest){
        //printf("\n\nEntrou isNotRestricted 2\n\n");
        if(node->city->ibge_code == target_city->ibge_code)
            return false;

        for(int j = 0; j < *size; j++)
            if(node->city->ibge_code == (nearest[j].city)->ibge_code)
                return false;
    }
    
    return true;
}

void swapNear( tnear * a, tnear * b ){
    tnear * temp = a;
    a = b;
    b = temp;
}

void addNear( tnear * nearest, kd_node * node, tcity * target, short * size, short max ){
    
    printf("\ncod: %d", node->city->ibge_code);
    //printf("\n\nEntrou addNear\n\n");
    nearest[*size].city = node->city;
    nearest[*size].distance = getCityDistance(node->city, target);

    //printf("\ncod: %d", (nearest[*size].city)->ibge_code);
    //printf("\ndist: %f\n", nearest[*size].distance);

    for(short i = 0; i < (*size)-1; i++){
        for(short j = 1; j < (*size); j++){
            if(nearest[i].distance > nearest[j].distance)
                swapNear((nearest+i), (nearest+j));
        }
    }

    (*size)++;

   //printf("\n\nPassou addNear\n\n");
}

// Verifica se em algum eixo a cidade a é mais próxima da cidade target do que a cidade b
bool possiblyCloser( kd_node * a, kd_node * b, tcity * target ){

    if(fabs(a->city->latitude - target->latitude) < fabs(b->city->latitude - target->latitude) || fabs(a->city->longitude - target->longitude) < fabs(b->city->longitude - target->longitude))
        return true;
    return false;
}

// Retorna o node da cidade válida mais próxima
kd_node * getNearestCity( kd_node * tree_node, tcity * target_city, tnear * found_cities, bool axis, short * size, short max ){

    if(tree_node == NULL)
        return NULL;

    kd_node * possible_best;
    kd_node * possible_worst;

    if(axis == 0 ? (tree_node->city->latitude < target_city->latitude) : (tree_node->city->longitude < target_city->longitude)){
        possible_best = getNearestCity(tree_node->right, target_city, found_cities, !axis, size, max);
        possible_worst = tree_node->right;
    }else{
        possible_best = getNearestCity(tree_node->left, target_city, found_cities, !axis, size, max);
        possible_worst = tree_node->left;
    }

    if(possible_best == NULL && tree_node)
        possible_best = tree_node;

    double best_distance = getCityDistance(possible_best->city, target_city);
    double tree_distance = getCityDistance(tree_node->city, target_city);
    double worst_distance;

    if(tree_node && isNotRestricted(found_cities, tree_node, target_city, size)){

        if(best_distance > tree_distance){
            possible_best = tree_node;
            best_distance = tree_distance;
        }
    }

    if(possible_best == NULL && possible_worst)
        possible_best = possible_worst;

    if(possible_worst && isNotRestricted(found_cities, possible_worst, target_city, size)){

        if(possiblyCloser(possible_worst, possible_best, target_city)){
            
            possible_worst = getNearestCity(possible_worst, target_city, found_cities, !axis, size, max);
            worst_distance = getCityDistance(possible_worst->city, target_city);

            if(best_distance > worst_distance){
                possible_best = possible_worst;
                best_distance = worst_distance;
            }
        }
    }

    if(possible_best && isNotRestricted(found_cities, possible_best, target_city, size))
        return possible_best;
    
    printf("\nRetornando nulo\n");
    return NULL;
}

// Retorna uma AVL com as n cidades mais próximas
tnear * getNearestCities( kd_tree * tree, thash * hash, uint32_t ibge_code, short n_nearest_cities ){

    printf("\nProcurando a cidade alvo..\n");
    tcity * target_city = getCity(hash, ibge_code);

    if(target_city == NULL)
        return NULL;

    tnear * nearest = (tnear *) calloc(n_nearest_cities, sizeof(tnear));

    if(nearest == NULL){
        printf("\nNão foi possível alocar cidades.\n");
        return NULL;
    }

    short actual_size = 0;
    short * size = &actual_size;

    printf("\nProcurando as %hu cidades mais próximas\n", n_nearest_cities);

    for(int i = 0; i < n_nearest_cities; i++)
        addNear(nearest, getNearestCity(tree->root, target_city, nearest, 0, size, n_nearest_cities), target_city, size, n_nearest_cities);

    printf("\nCidades encontradas.\n");


    return nearest;
}

// Printa todas as cidades da árvore em ordem crescente de distância em relação à cidade solicitada
void displayCities( tnear * nearest, short n_nearest_cities ){

    if(nearest){
        printf("\nAs %hu cidades mais próximas são: \n\n", n_nearest_cities);
        printf("código IBGE: %d", (nearest[0].city)->ibge_code);
    
        for(short i = 0; i < n_nearest_cities; i++)
            printf("\n%huº - código IBGE: %d - distância: %lf.", i+1, (nearest[i].city)->ibge_code, sqrt((nearest[i].distance)));
    }
}

#endif