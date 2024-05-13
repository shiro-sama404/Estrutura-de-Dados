/*

    Àrvore AVL

    Será usada pela árvore KD para armazenar as n cidades mais próximas
*/
#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdio.h>

#include "City.h"

typedef struct _avl_node{
    tcity * city;
    uint16_t height;
    double distance;
    struct _avl_node * left;
    struct _avl_node * right;
}avl_node;

typedef struct{
    avl_node * root;
}avl_tree;

/*

    Funções básicas da árvore AVL

*/

// Retorno do maior valor entre 2 unsigned shorts
uint16_t max( uint16_t a, uint16_t b ){

    return (a > b) ? a : b;
}

// Extração da altura de um nó
uint16_t getHeight( avl_node * node ){

    if(node)
        return node->height;

    return 0;
}

/* Notação usada para as rotações:

     A                         B
x         B               A         z
      y       z       x       y

*/

// Rotação a direita
avl_node * rotateRight(avl_node * B){
    avl_node * A = B->left;
    avl_node * y = A->right;

    A->right = B;
    B->left = y;

    A->height = getHeight(A);
    B->height = getHeight(B);

    return A;
}

// Rotação a esquerda
avl_node * rotateLeft(avl_node * A){
    avl_node * B = A->right;
    avl_node * y = B->left;

    B->left = A;
    A->right = y;

    A->height = getHeight(A);
    B->height = getHeight(B);

    return B;
}

// Extração do fator de balanceamento de um nó
uint16_t getBalanceFactor(avl_node * node){
    return getHeight(node->left) - getHeight(node->right);
}

// Aloca um node para árvore kd e retorna o sucesso ou fracasso da alocação
avl_node * newAVLNode( tcity * city){

    avl_node * new = (avl_node *) malloc(sizeof(avl_node));

    if(new){

        new->city = city;
        new->height = 0;
        new->left = NULL;
        new->right = NULL;

        return new;
    }
    return NULL;
}

// Inserção na AVL
avl_node * InsertInAVL( avl_node * tree_node, avl_node * new ){

    //printf("\nEntrou no insert AVL\n");
    
    // Inserção
    if(tree_node == NULL)
        return new;

    if (new->distance > tree_node->distance) 
        tree_node->right = InsertInAVL(tree_node->right, new);
    else
        tree_node->left = InsertInAVL(tree_node->left, new);

    //tree_node->height = getHeight(tree_node->left);
    //tree_node->height = max(getHeight(tree_node->left), getHeight(tree_node->right)) + 1;
  
    // Checando Balanceamento
    //short balance = getBalanceFactor(tree_node);
  
    /*printf("\nEntrou 1\n");
    // Esquerda -> esquerda
    if (balance > 1 && new->distance < tree_node->left->distance)
        return rotateRight(tree_node);

    printf("\nEntrou 2\n");
    // direita -> direita
    if (balance < -1 && new->distance > tree_node->right->distance)
        return rotateLeft(tree_node);

    printf("\nEntrou 3\n");
    // Esquerda -> direita
    if (balance > 1 && new->distance > tree_node->left->distance) {
        tree_node->left =  rotateLeft(tree_node->left);
        return rotateRight(tree_node);
    } 
  
    printf("\nEntrou 4\n");
    // Direita -> esquerda
    if (balance < -1 && new->distance < tree_node->right->distance) {
        tree_node->right = rotateRight(tree_node->right);
        return rotateLeft(tree_node);
    }*/
  
    //printf("\nSaiu do insert AVL\n");
    return tree_node;
}

// Busca binária na AVL
avl_node * SearchInAVL(avl_tree * tree, double distance, uint32_t ibge_code){

    avl_node * temp = tree->root;

    while(temp){

        if(distance == temp->distance && ibge_code == temp->city->ibge_code){
            return temp;
        }

        if(distance > temp->distance)
            temp = temp->right;
        else
            temp = temp->left;
    }

    //printf("\nNão foi possível encontrar o nó\n");
    return NULL;
}

// Destrói todos os nodes a partir de um node (deve ser chamado a partir da raíz)
void DestroyAVLNodes(avl_node * node){

    if(node){
        DestroyAVLNodes(node->left);
        DestroyAVLNodes(node->right);
        free((avl_node *) node);
    }
}

#endif