
#include "simbolo.h"

simbolo_t* criar_simbolo(char* id) {
    simbolo_t *s = (simbolo_t*) malloc(sizeof(simbolo_t));
    strcpy(s->id, id);
    return s;
}

int get_deslocamento(simbolo_t *simbolo) {
    switch (simbolo->categoria) {
        case VARIAVEL_SIMPLES:
            return simbolo->variavel.deslocamento;
        case PARAMETRO_FORMAL_VALUE:
            return simbolo->parametro.deslocamento;
        case PARAMETRO_FORMAL_REF:
            return simbolo->parametro.deslocamento;
        case PROCEDIMENTO:
            return simbolo->procedimento.deslocamento;
        case FUNCAO:
            return simbolo->procedimento.deslocamento;
    }
}

int proc_get_qtd_param(simbolo_t *s) {
    int qtd = 0;
    simbolo_t *param = s->procedimento.primeiro_parametro;
    while (param != NULL) {
        qtd++;
        param = param->parametro.proximo_parametro;
    }
    return qtd;
}

void proc_adiciona_param(simbolo_t* proc, simbolo_t *param) {
    if (proc->procedimento.primeiro_parametro == NULL) {
        proc->procedimento.primeiro_parametro = param;
        return;
    }

    simbolo_t *p = proc->procedimento.primeiro_parametro;

    while (p->parametro.proximo_parametro != NULL) {
        p = p->parametro.proximo_parametro;
    }

    p->parametro.proximo_parametro = param;
}


simbolo_t* proc_get_param_at(simbolo_t *proc, int indice) {
    simbolo_t *param = proc->procedimento.primeiro_parametro;
    int i = 0;
    while (param != NULL) {
        if (i == indice) {
            return param;
        }
        i++;
        param = param->parametro.proximo_parametro;
    }
    return NULL;
}
