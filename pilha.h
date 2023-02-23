#ifndef pilha_h
#define pilha_h

#include <stdio.h>
#include "simbolo.h"
#include "infos_compilador.h"
#include "infos_chamada_subrot.h"

typedef struct item_pilha {
    struct item_pilha *prev;
    union {
        int n_simbolos;
        char label[4];
        simbolo_t *simbolo;
        infos_compilador_t *infos;
        infos_chamada_subrot_t *infos_chamada_subrot;
        tipo_var tipo;
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
int pilha_busca_duplicata_simbolo(pilha_t *pilha, char *id, int nivel_lexico);

void pilha_push_label(pilha_t *pilha, char *label);
char* pilha_pop_label(pilha_t *pilha);
char* pilha_peek_label(pilha_t *pilha);

void pilha_push_infos(pilha_t *pilha, infos_compilador_t *infos);
infos_compilador_t* pilha_pop_infos(pilha_t *pilha);
infos_compilador_t* pilha_peek_infos(pilha_t *pilha);

void pilha_push_chamada_subrot(pilha_t *pilha, infos_chamada_subrot_t *infos_chamada_subrot);
infos_chamada_subrot_t *pilha_pop_chamada_subrot(pilha_t *pilha);
infos_chamada_subrot_t *pilha_peek_chamada_subrot(pilha_t *pilha);

void pilha_push_tipo(pilha_t *pilha, tipo_var tipo);
tipo_var pilha_pop_tipo(pilha_t *pilha);
tipo_var pilha_peek_tipo(pilha_t *pilha);


#endif /* pilha_h */
