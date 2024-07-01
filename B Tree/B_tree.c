#include <stdio.h>
#include <stdlib.h>

#include "B_tree.h"

int menor(int a, int b){
    return (a < b) ? a : b;
}

void remove_chave(tnode * x, tchave ik){
    
    for(int i = ik; i < x->n_chaves-1; i++)
        x->chaves[i] = x->chaves[i+1];
    
    x->n_chaves--;
}

tchave sucessor(tnode * x, tchave k){

    int ik = procura_chave(x, k);
    tnode * aux = x->filhos[ik+1];

    while(!aux->folha)
        aux = x->filhos[0];
    
    return aux->chaves[0];
}

tchave predecessor(tnode * x, tchave k){

    int ik = procura_chave(x, k);
    tnode * aux = x->filhos[ik];

    while(!aux->folha)
        aux = x->filhos[x->n_chaves];
    
    return aux->chaves[aux->n_chaves-1];
}

tnode * aloca_node(){

    tnode * aux = (tnode *) malloc(sizeof(tnode));

    if(aux){
        aux->n_chaves = 0;
        aux->folha = false;
    }

    return aux;
}

void btree_constroi(btree * arv, int t){

    arv->t = t;
    arv->raiz = aloca_node(arv);
    arv->raiz->folha = true;
}

bool btree_split(btree * arv, tnode * x, int i){

    tnode * z = aloca_node();
    tnode * y = x->filhos[i];
    int t = arv->t;
    int j;
    
    if (z){

        z->folha = y->folha;
        z->n_chaves = t-1;

        for (j = 0; j < t - 1; j++)
            z->chaves[j] = y->chaves[j+t];

        if (!y->folha)   /* copia os ponteiros caso z e y nao sejam folhas*/
            for (j = 0; j < t; j++)
                z->filhos[j] = y->filhos[j+t];

        y->n_chaves = t-1;
        for (j = x->n_chaves; j >= i+1; j--)
            x->filhos[j+1] = x->filhos[j-1];
        
        x->filhos[i+1] = z;

        for(j = x->n_chaves; j > i; j--)
            x->chaves[j] = x->chaves[j-1];

        x->chaves[i] = y->chaves[t-1];
        x->n_chaves++;
        x->folha = false;
        
        return true;
    }

    return false;
}

bool btree_insere_naocheio(btree *arv, tnode * x, tchave k){

    int i = x->n_chaves;
    int t = arv->t;

    if (x->folha){

        while(i > 0 && k < x->chaves[i-1]){
            x->chaves[i] = x->chaves[i-1];
            i--;
        }

        x->chaves[i] = k;
        x->n_chaves++;
        return true;
    }else{

        i--;
        while(i >= 0 && k < x->chaves[i])
            i--;
        i++;

        if (x->filhos[i]->n_chaves == 2 * t - 1){
            btree_split(arv, x, i);
            if (k > x->chaves[i])
                i++;
        }
        return btree_insere_naocheio(arv, x->filhos[i], k);
    }
}

bool btree_insere(btree *arv, tchave k){

    tnode * r = arv->raiz;
    int t = arv->t;
    
    if (r->n_chaves == 2 * t - 1){

        tnode * s = aloca_node();
        arv->raiz = s;
        s->filhos[0] = r;
        
        if (btree_split(arv,s,0))
           return btree_insere_naocheio(arv,s,k);
    }else
        return btree_insere_naocheio(arv,r,k);
}

int procura_chave(tnode * x, tchave k){

    for(int i = 0; i < x->n_chaves; i++)
        if (x->chaves[i] == k)
            return i;
    
    return -1;
}


int procura_ic(tnode *x, tchave k){

    int i = x->n_chaves - 1;

    while (k < x->chaves[i] && i >= 0)
        i--;
    
    return ++i;
}

void merge(btree * arv, tnode * x, int cpos){
    
    tnode * y = x->filhos[cpos];
    tnode * z = x->filhos[cpos+1];

    int i;

    y->chaves[y->n_chaves-1] = x->chaves[cpos];


    for(i = 0; i < z->n_chaves; i++)
        y->chaves[y->n_chaves+1+i] = z->chaves[i];

    if(!y->folha)
        for(i = 0; i < z->n_chaves; i++)
            y->filhos[y->n_chaves+1+i] = z->filhos[i];

    y->n_chaves = 2 * (arv->t)-1;

    for(i = cpos; i < x->n_chaves-1; i++){
        x->chaves[i] = x->chaves[i+1];
        x->filhos[i+1] = x->filhos[i+2];
    }

    x->n_chaves--;

    if(x->n_chaves == 0){

        for(i = 0; i < y->n_chaves; i++){
            x->chaves[i] = y->chaves[i];
            x->filhos[i] = y->filhos[i];
        }

        x->filhos[i] = y->filhos[i];
        x->folha = y->folha;
        x->n_chaves = 2 * (arv->t) - 1;
        free(y);
    }

    free(z);
}

void pega_emprestado_irmao(tnode *x, int cpos, int irmao){

    int i;
    tnode *y,*z;

    if (cpos < irmao){ /* irmao a direita*/

        y = x->filhos[cpos];
        z = x->filhos[cpos+1];
        y->chaves[y->n_chaves] = x->chaves[cpos];
        x->chaves[cpos] = z->chaves[0];
        y->filhos[y->n_chaves+1] = z->filhos[0];

        for (i =  0;i < z->n_chaves-1; i++){
            z->chaves[i] = z->chaves[i+1];
            z->filhos[i] = z->filhos[i+1];
        }

        z->filhos[i] = z->filhos[i+1];
        z->n_chaves--;
        y->n_chaves++;
    }else{ /*irmao a esquerda*/

        cpos--;
        y = x->filhos[cpos];
        z = x->filhos[cpos+1];

        for (i=z->n_chaves;i>0;i--){
            z->chaves[i] = z->chaves[i-1];
            z->filhos[i+1] = z->filhos[i];
        }

        z->chaves[0] = x->chaves[cpos];
        z->filhos[0] = y->filhos[y->n_chaves];
        x->chaves[cpos] = y->chaves[y->n_chaves-1];

        y->n_chaves--;
        z->n_chaves++;
    }
}

int pega_irmao_maior(tnode * x, int cpos){

    int ant = cpos -1;
    int pos = cpos +1;

    if (ant < 0)
        return pos;

    if (pos > x->n_chaves)
        return ant;

    if (x->filhos[ant]->n_chaves > x->filhos[pos]->n_chaves)
        return ant;

    return pos;
}

bool _btree_remove(btree *arv, tnode * x, tchave k){

    int ik = procura_chave(x,k);  /* key position */
    int t = arv->t;
    int i;
    int iirmao_maior;
    tnode *y,*z;
    tchave klinha;

    if (ik >= 0){ /* k in x*/

        if (x->folha){ /* Caso a - simplesmente remove*/
            remove_chave(x,ik);
            return true;
        }else{ 

            y = x->filhos[ik];
            z = x->filhos[ik+1];
            if (y->n_chaves >= t){ /* b  tenta no irmao esquerda*/
                x->chaves[ik] = predecessor(x, k);
                return _btree_remove(arv, y, x->chaves[ik]);
            }else if (z->n_chaves >= t){ /* b tenta no irmao direta*/
                x->chaves[ik] = sucessor(x,k);
                return _btree_remove(arv, z, x->chaves[ik]);
            }else if ((y->n_chaves == t -1) && (z->n_chaves == t-1)){ /* c faz merge */
                merge(arv, x, ik);
                return _btree_remove(arv, x, k);
            }
        }
    }else{ /* nos intermediarios */

        if (x->folha)
            return false;
        else{
            i = procura_ic(x, k);

            if (x->filhos[i]->n_chaves == t-1){

                iirmao_maior = pega_irmao_maior(x, i);

                if (x->filhos[iirmao_maior]->n_chaves >= t){            /* a */
                    pega_emprestado_irmao(x, i, iirmao_maior);
                    return _btree_remove(arv, x->filhos[i], k);
                }else if (x->filhos[iirmao_maior]->n_chaves == t-1){   /* b */
                    merge(arv, x, menor(i,iirmao_maior)); 
                    return _btree_remove(arv, x, k);
                }
            }else
                return _btree_remove(arv,x->filhos[i],k);
        }
    }
}

int btree_remove(btree *arv, tchave k){

    printf("Removendo %d\n",k);
    return _btree_remove(arv, arv->raiz,k);
}