//#include <stdio.h>
#include "..\Bibliotecas\AVL_Tree.h"

uint16_t max( uint16_t a, uint16_t b ){

    return (a > b) ? a : b;
}

uint16_t getHeight( avl_node * node ){

    if(node == NULL)
        return 0;

    return max(getHeight(node->left), getHeight(node->right));
}

/* Notação usada para as rotações:

     A                         B
x         B               A         z
      y       z       x       y

*/

avl_node * rotateRight(avl_node * B){
    avl_node * A = B->left;
    avl_node * y = A->right;

    A->right = B;
    B->left = y;

    A->height = getHeight(A);
    B->height = getHeight(B);

    return A;
}

avl_node * rotateLeft(avl_node * A){
    avl_node * B = A->right;
    avl_node * y = B->left;

    B->left = A;
    A->right = y;

    A->height = getHeight(A);
    B->height = getHeight(B);

    return B;
}

uint16_t getBalanceFactor(avl_node * node){
    return getHeight(node->left) - getHeight(node->right);
}

avl_node * InsertInAVL(avl_tree * tree, tcity * city){

    avl_node * node = (avl_node *) malloc(sizeof(avl_node));

    if(node)
        return false;

    node->city = city;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;

    if(tree->root == NULL){
        tree->root = node;
    }else{

        avl_node * temp = tree->root;

        while(temp != NULL){

            if(node->city->latitude >= temp->city->latitude){

                if(temp->right)
                    temp = temp->right;
                else{
                    temp->right = node;
                    break;
                }
            }else{

                if(temp->left)
                    temp = temp->left;
                else{
                    temp->left = node;
                    break;
                }
            }
        }
    }
}

tcity * SearchInAVL(avl_tree * tree, double distance){

    avl_node * temp = tree->root;

    while(temp){

        if(temp->distance == distance){
            return temp->city;
        }
    }
}