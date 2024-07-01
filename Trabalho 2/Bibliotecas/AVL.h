#ifndef __AVL__
#define __AVL__

#include "Hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct _register{
    uint32_t ibge_code;
    void * content;
    struct _register * next;
}tregister;

typedef struct _node{
    tregister * cities_register;
    struct _node * father;
    struct _node *left;
    struct _node *right;
    uint16_t height;
}tnode;

typedef struct _tree{
    tnode * root;
}tAVL;

typedef struct _query{
    uint32_t ibge_code;
    struct _query * next;
}tquery;

uint16_t max(uint16_t a, uint16_t b){
    return a > b ? a : b;
}

// Faz comparações entre um node e uma cidade (result < 0: A é menor, 0: iguais, result > 1: A é maior)
short compareByField( tregister * reg, tcity * city, short field ){

    // Compara os nomes (strings)
    if(field == 0)
        return strcmp((char *)reg->content, city->name);
    // Compara latitudes/longitudes (floats)
    if(field < 3){
        if(*((double *)reg->content) == (field == 1 ? city->latitude : city->longitude))
            return 0;
        return (short)(*((double *)reg->content) - (field == 1 ? city->latitude : city->longitude));
    }
    // Compara códigos UF/DDDs (inteiros)
    if(*((uint8_t *)reg->content) == (field == 3 ? city->uf_code : city->ddd))
        return 0;
    return *((uint8_t *)reg->content) - (field == 3 ? city->uf_code : city->ddd);

    return 0;
}

// Atribui baseado em um campo
void * setByField( tcity * city, short field ){

    if(field == 0)
        return (void *)&city->name;
    if(field == 1)
        return (void *)&city->latitude;
    if(field == 2)
        return (void *)&city->longitude;
    if(field == 3)
        return (void *)&city->uf_code;
    if(field == 4)
        return (void *)&city->ddd;
    return 0;
}

// Adiciona um novo registro ao conjunto de registros de um node
void addToRegister( tregister * register_element, tcity * city, short field ){
    
    tregister * temp = register_element;

    while(temp->next)
        temp = temp->next;

    temp->next = (tregister *) malloc(sizeof(tregister));
    temp = temp->next;

    temp->ibge_code = city->ibge_code;
    temp->content = setByField(city, field);
    temp->next = NULL;
}

uint16_t height( tnode * node ){
    if(node)
        return max(height(node->left), height(node->right)) + 1;
    return 0;
}

short getBalance(tnode * node){
    return height(node->left) - height(node->right);
}

tnode * rotateLeft(tnode * A){
    tnode * B = A->right;
    tnode * y = B->left;

    B->left = A;
    A->right = y;

    return B;
}

tnode * rotateRight(tnode * B){
    
    tnode * A = B->left;
    tnode * y = A->right;

    A->right = B;
    B->left = y;

    return A;
}

// Cria um novo node
tnode * newNode( tnode * father_node, tcity * city, short field ){

    tnode * new_node = (tnode*) malloc(sizeof(tnode));

    // Aloca e configura um novo registro
    new_node->cities_register = (tregister *) malloc(sizeof(tregister));
    new_node->cities_register->ibge_code = city->ibge_code;
    new_node->cities_register->content = setByField(city, field);
    new_node->cities_register->next = NULL;

    new_node->father = father_node;
    new_node->right = NULL;
    new_node->left = NULL;
    new_node->height = 0;

    return new_node;
}

// Faz o balanceamento da AVL
void Balance( tnode * node ){

    node->height = height(node);
    short balance = getBalance(node);

    // Direita
    if(balance > 1){
        
        // Esquerda
        if(getBalance(node->left) < 0)
            node->left = rotateLeft(node->left);
        
        node = rotateRight(node);
    
    // Esquerda
    }else if(balance < -1){

        // Direita
        if(getBalance(node->right) > 0)
            node->right = rotateRight(node->right);

        node = rotateLeft(node);
    }
}

// Realiza a inserção de um novo registro
tnode * Insert( tnode * father, tnode * node, tcity * city, short field ){

    if(node == NULL)
        return newNode(father, city, field);

    short comparison = compareByField(node->cities_register, city, field);

    if(comparison < 0)
        node->right = Insert(node, node->right, city, field);
    else if(comparison > 0)
        node->left = Insert(node, node->left, city, field);
    else{
        addToRegister(node->cities_register, city, field);
    }
        
    Balance(node);
    
    return node;
}

// Inicializa uma AVl a partir de uma tabela hash
bool InitializeAVL( tAVL * tree, thash * hash, short field ){

    if (hash->table == NULL)
        return true;

    for(int i = 0; i < hash->maxSize; i++)
        if(hash->table[i])
            tree->root = Insert(NULL, tree->root, hash->table[i], field);

    return true;
} 

tquery * newQuery(uint32_t ibge_code){
    tquery * new = (tquery *) malloc(sizeof(tquery));
    new->ibge_code = ibge_code;
    new->next = NULL;
    return new;
}
// Adiciona os resultados de uma nova query à query resultado (eliminando repetições)
void addToQuery( tquery * query, tregister * reg){

    tquery * query_element = query;
    tregister * reg_element = reg;

    // Se a query não estiver vazia então avalia se há repetições
    if(query){

        tquery * query_element = query;

        bool repeated;
        while(reg_element){

            repeated = false;
            while(query_element){
                if(query_element->ibge_code == reg_element->ibge_code){
                    repeated = true;
                    break;
                }
            }

            if(!repeated)
                query_element->ibge_code = reg_element->ibge_code;

            reg_element = reg_element->next;
            query_element = query_element->next;
        }
    // Se não, simplesmente adiciona
    }else{
        
        while(reg){
            query_element = newQuery(reg->ibge_code);
            query_element = query_element->next;
            reg = reg->next;
        }
    }
}

tnode * successor( tnode * node ){

    if(node->right){
        return node->right;
    }
    
    if(node->father && node->father->cities_register->ibge_code > node->cities_register->ibge_code)
        return node->father->right;

    return node->father->father;
}

tnode * minNode( tnode * node, void * min, short field ){
    
    if(node->left == NULL)
        return node;

    // avl de nomes
    if(field == 0){
        if(strcmp(node->cities_register->content, min) < 0)
            return minNode(node->left, min, field);

    // avls do tipo double 
    }else if(field < 3){
        if(*((double *)node->cities_register->content) < *((double *)min))
            return minNode(node->left, min, field);

    // avls do tipo inteiro 
    }else if(field < 5){
        if(*((uint8_t *)node->cities_register->content) < *((uint8_t *)min))
            return minNode(node->left, min, field);
    }

    return node;
}

void Query( tquery * query, tnode * node, void * min, void * max, short field ){

    if(node == NULL)
        return;

    // avl de nomes
    if(field == 0){

        if(strcmp((char *)node->cities_register->content, (char *)min) < 0){
            Query(query, node->right, min, max, field);
            return;
        }

        // Encontra o min Node menor que o mínimo
        tnode * successor_node = successor(minNode(node->right, min, field));

        // Adiciona todos os registros de todos os sucessores menores que o máximo
        while(successor_node && strcmp((char *)node->cities_register->content, (char *)max) <= 0){
            addToQuery(query, successor_node->cities_register);
            successor_node = successor(successor_node);
        }

    // avls do tipo double 
    }else if(field < 3){

        if(*((double *)node->cities_register->content) < *((double *)min)){
            Query(query, node->right, min, max, field);
            return;
        }

        // Encontra o min Node menor que o mínimo
        tnode * successor_node = successor(minNode(node->right, min, field));

        // Adiciona todos os registros de todos os sucessores menores que o máximo
        while(successor_node && *((double *)node->cities_register->content) <= *((double *)max)){
            addToQuery(query, successor_node->cities_register);
            successor_node = successor(successor_node);
        }

    // avls do tipo inteiro 
    }else if(field < 5){

        if(*((uint8_t *)node->cities_register->content) < *((uint8_t *)min)){
            Query(query, node->right, min, max, field);
            return;
        }

        // Encontra o min Node menor que o mínimo
        tnode * successor_node = successor(minNode(node->right, min, field));

        // Adiciona todos os registros de todos os sucessores menores que o máximo
        while(successor_node && *((uint8_t *)node->cities_register->content) <= *((uint8_t *)max)){
            addToQuery(query, successor_node->cities_register);
            successor_node = successor(successor_node);
        }
    }
}

// Destrói todos os nós de uma raiz
void destroyRoot( tnode * node ){

    if(node){
        destroyRoot(node->left);
        destroyRoot(node->right);
        free((tnode *) node);
    }
}

#endif