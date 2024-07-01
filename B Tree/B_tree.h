#ifndef b_tree_h
#define b_tree_h

#include <stdbool.h>

typedef int tchave;

typedef struct tnode{
    bool folha;
    int n_chaves;
    tchave * chaves;
    struct tnode ** filhos;
}tnode;

typedef struct btree{
    tnode * raiz;
    int t;
}btree;

bool btree_split(btree * arv, tnode * x, int i);
bool btree_insere_naocheio(btree * arv, tnode * x, tchave k);
bool btree_insere(btree * arv, tchave k);

tnode * aloca_node(btree * arv);
void btree_constroi(btree * arv, int t);

int procura_chave(btree * arv, tchave k);
int pega_irmao_maior(tnode * x, int cpos);
int procura_ic(tnode * x, tchave k);

void merge(btree * arv, tnode * x, tchave k);
void pega_emprestado_irmao(tnode * x, int cpos, int irmao);
int btree_remove(btree * arv, tchave k);

#endif