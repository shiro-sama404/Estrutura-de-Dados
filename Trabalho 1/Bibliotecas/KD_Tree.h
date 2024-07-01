#ifndef KDTREE_H
#define KDTREE_H

#include "..\Bibliotecas\Hash.h"

typedef struct _kd_node{
    tcity * city;
    uint16_t height;
    struct _kd_node * left;
    struct _kd_node * right;
}kd_node;

typedef struct{
    kd_node * root;
}kd_tree;

typedef struct{
    tcity * city;
    double distance;
}tnear;

/*               Funções básicas da árvore KD                  */

// Aloca um node para árvore kd e retorna o sucesso ou fracasso da alocação
kd_node * newKDNode( tcity * city ){

    kd_node * new = (kd_node *) malloc(sizeof(kd_node));

    if(new){

        new->city = city;
        new->height = 0;
        new->left = NULL;
        new->right = NULL;

        return new;
    }

    printf("\nNão foi possível alocar novo node\n");
    return NULL;
}

// Inserção na árvore KD
bool InsertInKDTree( kd_tree * tree, tcity * city ){

    if(city == NULL)
        return false;

    kd_node * new = newKDNode(city);

    if(new == NULL)
        return false;

    if(tree->root == NULL)
        tree->root = new;
    else{

        kd_node * temp = tree->root;
        bool axis = 0; // 0 = latitudade e 1 = longitudade

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
    hash->table = (tcity **) calloc(11131, sizeof(tcity *));

    if (hash->table == NULL || !defineDeleted(hash->deleted))
        return false;
    
    hash->actualSize = 0;
    hash->maxSize = 11131;

    //  Leitura do arquivo json e inserção dos dados na árvore
    FILE * jsonFile = fopen("../../src/municipios.json", "r");

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
void DestroyNodes( kd_node * node ){

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
bool isRestricted( tnear ** nearest_cities, kd_node * node, tcity * target_city, uint16_t * size ){

    if(node == NULL)
        return true;
    
    if(node->city->ibge_code == target_city->ibge_code)
        return true;

    for(int i = 0; i < * size; i++)
        if(node->city->ibge_code == ((*nearest_cities)[i].city)->ibge_code)
            return true;
    
    return false;
}

// Verifica se em algum eixo a distância da cidade A até a cidade target é menor que dist
bool possiblyCloser( kd_node * A, double dist, tcity * target ){

    //if(A == NULL)
        //printf("\nUm é null\n");
    if(fabs(A->city->latitude - target->latitude) < dist)
        return true;
    if(fabs(A->city->longitude - target->longitude) < dist)
        return true;
    return false;
}

// Troca duas cidades próximas
void swapNear( tnear * a, tnear * b ){
    tnear temp = *a;
    *a = *b;
    *b = temp;
}

// Adiciona uma cidade a um array de cidades mais próximas em ordem
void addNear( tnear ** nearest_cities, kd_node * node, tcity * target_city, uint16_t * size ){
    
    if(node == NULL)
        printf("NODE NULL");

    if(isRestricted(nearest_cities, node, target_city, size))
        printf("\ne restrito\n");
    
    tnear new;
    new.city = node->city;
    new.distance = getCityDistanceSQRT(node->city, target_city);
    uint16_t i = (*size);
    (*nearest_cities)[i] = new;

    while(i > 0 && (*nearest_cities)[i].distance < (*nearest_cities)[i-1].distance){
        swapNear(&(*nearest_cities)[i], &(*nearest_cities)[i-1]);
        i = i - 1;
    }

    (*size) = (*size) + 1;
}

// Retorna o node da cidade válida mais próxima
kd_node * getNearestCity( kd_node * tree_node, tcity * target_city, tnear ** found_cities, bool axis, uint16_t * size, uint16_t max ){
    
    if(tree_node == NULL)
        return NULL;

    kd_node * possible_best;
    kd_node * possible_worst;

    if(axis == false ? (target_city->latitude > tree_node->city->latitude) : (target_city->longitude > tree_node->city->longitude)){
        possible_best = getNearestCity(tree_node->right, target_city, found_cities, !axis, size, max);
        possible_worst = tree_node->left;
    }else{
        possible_best = getNearestCity(tree_node->left, target_city, found_cities, !axis, size, max);
        possible_worst = tree_node->right;
    }

    // Verifica a restrição do node atual
    bool nodeIsRestricted = isRestricted(found_cities, tree_node, target_city, size);
    bool worstIsRestricted = isRestricted(found_cities, possible_worst, target_city, size);

    // Melhor distância
    double best_distance;
    
    // Verifica se o mais próximo válido é o possível melhor ou o próprio node
    if(possible_best && nodeIsRestricted)

        best_distance = getCityDistance(possible_best->city, target_city);
        
    else if(possible_best == NULL && nodeIsRestricted == false){

        possible_best = tree_node;
        best_distance = getCityDistance(tree_node->city, target_city);
    }else if(possible_best && nodeIsRestricted == false){

        double tree_distance = getCityDistance(tree_node->city, target_city);
        best_distance = getCityDistance(possible_best->city, target_city);
        if(best_distance > tree_distance){
            possible_best = tree_node;
            best_distance = tree_distance;
        }
    }

    // Verifica se há chance da possível pior rota conter a cidade mais próxima
    if(possible_best == NULL){

        if(worstIsRestricted)
            possible_best = getNearestCity(possible_worst, target_city, found_cities, !axis, size, max);
        else
            possible_best = possible_worst;
        
    }else if(possible_worst && possiblyCloser(tree_node, best_distance, target_city)){
        
        // Pega a melhor cidade da possível pior rota e compara
        possible_worst = getNearestCity(possible_worst, target_city, found_cities, !axis, size, max);

        if(possible_worst == NULL)
            return possible_best;

        double worst_distance = getCityDistance(possible_worst->city, target_city);

        if(best_distance > worst_distance){
            possible_best = possible_worst;
            best_distance = worst_distance;
        }
    }
    
    return possible_best;
}

// Retorna um array com as n cidades mais próximas
tnear * getNearestCities( kd_tree * tree, thash * hash, uint32_t ibge_code, uint16_t n_nearest_cities ){

    printf("\nProcurando a cidade alvo..\n");
    tcity * target_city = getCity(hash, ibge_code);

    if(target_city == NULL)
        return NULL;

    tnear * nearest_cities = (tnear *) calloc(n_nearest_cities, sizeof(tnear));

    if(nearest_cities == NULL){
        printf("\nNao foi possivel alocar as cidades.\n");
        return NULL;
    }

    uint16_t actual_size = 0;
    uint16_t * size = &actual_size;

    printf("\nProcurando as %hu cidades mais proximas..\n", n_nearest_cities);
    //kd_node * nha = tree->root;
    for(int i = 0; i < n_nearest_cities; i++)
        addNear(&nearest_cities, getNearestCity(tree->root, target_city, &nearest_cities, false, size, n_nearest_cities), target_city, size);
        //addNear(&nearest_cities, nha, target_city, size);
    printf("\nCidades encontradas.\n");

    return nearest_cities;
}

// Printa todas as cidades da árvore em ordem crescente de distância em relação à cidade solicitada
void displayCities( tnear * nearest_cities, uint16_t n_nearest_cities ){

    if(nearest_cities){
        printf("\nAs %hu cidades mais proximas sao: \n", n_nearest_cities);
    
        for(short i = 0; i < n_nearest_cities; i++)
            printf("\n%hu - codigo IBGE: %d - distancia: %lf.", i+1, (nearest_cities[i].city)->ibge_code, (nearest_cities[i].distance));
    }
}

#endif