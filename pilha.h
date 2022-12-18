#ifndef pilha_h
#define pilha_h

#include <stdio.h>
#include "simbolo.h"
#include "infos_compilador.h"

typedef struct item_pilha {
    struct item_pilha *prev;
    union {
        int n_simbolos;
        char label[4];
        simbolo_t *simbolo;
        infos_compilador_t *infos;
    };
} item_pilha_t;

typedef struct {
    item_pilha_t *top;
    int length;
} pilha_t;

pilha_t* pilha_create();
int pilha_is_empty(pilha_t *pilha);
void pilha_print(pilha_t *pilha, void print(item_pilha_t*));

void pilha_push_simbolo(pilha_t *pilha, simbolo_t *simbolo);
simbolo_t* pilha_pop_simbolo(pilha_t *pilha);
simbolo_t* pilha_peek_simbolo(pilha_t *pilha);
simbolo_t* pilha_get_by_id_simbolo(pilha_t *pilha, char *id);
void pilha_print_simbolo(item_pilha_t *item);
void print_simbolo(simbolo_t *s);

void pilha_push_label(pilha_t *pilha, char *label);
char* pilha_pop_label(pilha_t *pilha);
char* pilha_peek_label(pilha_t *pilha);

void pilha_push_infos(pilha_t *pilha, infos_compilador_t *infos);
infos_compilador_t* pilha_pop_infos(pilha_t *pilha);
infos_compilador_t* pilha_peek_infos(pilha_t *pilha);

void pilha_push_n(pilha_t *pilha, int n_symbs);
int pilha_pop_n(pilha_t *pilha);
int pilha_peek_n_symbs(pilha_t *pilha);

#endif /* pilha_h */
