
#ifndef symbol_h
#define symbol_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

typedef enum {
    VARIAVEL_SIMPLES,
    PARAMETRO_FORMAL_VALUE,
    PARAMETRO_FORMAL_REF,
    PROCEDIMENTO,
    FUNCAO,
} categoria_simbolo;

typedef struct simbolo {
    char id[TAM_TOKEN];
    categoria_simbolo categoria;
    int nivel_lexico;

    union {
        struct {
            int deslocamento;
        } variavel;

        struct {
            int deslocamento;
            struct simbolo *proximo_parametro;
        } parametro;

        struct {
            int deslocamento;
            char rotulo[4];
            struct simbolo *primeiro_parametro;
        } procedimento;
    };
} simbolo_t;

simbolo_t* criar_simbolo(char* id);

int get_deslocamento(simbolo_t *simbolo);

int proc_get_qtd_param(simbolo_t *s);
void proc_adiciona_param(simbolo_t* proc, simbolo_t *param);

simbolo_t* proc_get_param_at(simbolo_t *proc, int indice);

#endif /* symbol_h */
