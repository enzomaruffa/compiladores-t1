
#include "pilha.h"

// MARK: Pilha genérica
pilha_t* pilha_create(void) {
    pilha_t *new = malloc(sizeof(pilha_t));
    new->length = 0;
    new->top = NULL;
    return new;
}

int pilha_is_empty(pilha_t *pilha) {
    return pilha->length == 0;
}

void pilha_print(pilha_t *pilha, void print(item_pilha_t*)) {
    printf("–––––––––––––––––––––\n");
    for (item_pilha_t *item = pilha->top; item; item = item->prev) {
        print(item);
    }
    printf("–––––––––––––––––––––\n");
}

// MARK: Pilha de simbolos
void pilha_push_simbolo(pilha_t *pilha, simbolo_t *simbolo) {
    item_pilha_t *item = malloc(sizeof(item_pilha_t));
    item->simbolo = simbolo;
    item->prev = pilha->top;
    pilha->top = item;
    pilha->length++;
}

simbolo_t* pilha_pop_simbolo(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        pilha->top = item->prev;
        pilha->length--;
        simbolo_t *simbolo = item->simbolo;
        free(item);
        return simbolo;
    } else {
        return NULL;
    }
}

simbolo_t* pilha_peek_simbolo(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        return item->simbolo;
    } else {
        return NULL;
    }
}

simbolo_t* pilha_get_by_id_simbolo(pilha_t *pilha, char *id) {
    for (item_pilha_t *item = pilha->top; item; item = item->prev) {
        if (!strcmp(item->simbolo->id, id)) {
            return item->simbolo;
        }
    }
    return NULL;
}

void pilha_print_simbolo(item_pilha_t *item) {
    print_simbolo(item->simbolo);
}

void print_simbolo(simbolo_t* s) {
    switch (s->categoria) {
        case VARIAVEL_SIMPLES:
            printf("VS %s: %d %d\n", s->id, s->nivel_lexico, s->variavel.deslocamento);
            break;
        case PROCEDIMENTO:
            printf("PR %s: %d %s\n", s->id, s->nivel_lexico, s->procedimento.rotulo);
            break;
        case PARAMETRO_FORMAL_VALUE:
        case PARAMETRO_FORMAL_REF:
            printf("PF %s: %d %d\n", s->id, s->nivel_lexico, s->parametro.deslocamento);
            break;
        case FUNCAO:
            printf("FU %s: %d %d %s\n", s->id, s->nivel_lexico, s->procedimento.deslocamento, s->procedimento.rotulo);
            break;
        default:
            break;
    }
}

int pilha_busca_duplicata_simbolo(pilha_t *pilha, char *id, int nivel_lexico) {
    for (item_pilha_t *item = pilha->top; item; item = item->prev) {
        if (!strcmp(item->simbolo->id, id) && item->simbolo->nivel_lexico == nivel_lexico) {
            return 1;
        }
    }
    return 0;
}

// MARK: Label pilha
void pilha_push_label(pilha_t *pilha, char *label) {
    item_pilha_t *item = malloc(sizeof(item_pilha_t));
    strcpy(item->label, label);
    item->prev = pilha->top;
    pilha->top = item;
    pilha->length++;
}

char* pilha_pop_label(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        pilha->top = item->prev;
        pilha->length--;
        char *label = malloc(sizeof(char) * 4);
        strcpy(label, item->label);
        free(item);
        return label;
    } else {
        return NULL;
    }
}

char* pilha_peek_label(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        return item->label;
    } else {
        return NULL;
    }
}

// MARK: Pilha de infos
void pilha_push_infos(pilha_t *pilha, infos_compilador_t *infos) {
    item_pilha_t *item = malloc(sizeof(item_pilha_t));
    item->infos = infos;
    item->prev = pilha->top;
    pilha->top = item;
    pilha->length++;
}

infos_compilador_t* pilha_pop_infos(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        pilha->top = item->prev;
        pilha->length--;
        infos_compilador_t *infos = item->infos;
        free(item);
        return infos;
    } else {
        return NULL;
    }
}

infos_compilador_t* pilha_peek_infos(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        return item->infos;
    } else {
        return NULL;
    }
}

infos_compilador_t* pilha_get_infos_by_nivel_lexico(pilha_t *pilha, int nivel_lexico) {
    for (item_pilha_t *item = pilha->top; item; item = item->prev) {
        if (item->infos->nivel_lexico == nivel_lexico) {
            return item->infos;
        }
    }
    return NULL;
}

// Pilha de chamada de subrot
void pilha_push_chamada_subrot(pilha_t *pilha, infos_chamada_subrot_t *infos_chamada_subrot) { 
    item_pilha_t *item = malloc(sizeof(item_pilha_t));
    item->infos_chamada_subrot = infos_chamada_subrot;
    item->prev = pilha->top;
    pilha->top = item;
    pilha->length++;
}

infos_chamada_subrot_t *pilha_pop_chamada_subrot(pilha_t *pilha) { 
    item_pilha_t *item = pilha->top;
    if (item) {
        pilha->top = item->prev;
        pilha->length--;
        infos_chamada_subrot_t *infos_chamada_subrot = item->infos_chamada_subrot;
        free(item);
        return infos_chamada_subrot;
    } else {
        return NULL;
    }
}

infos_chamada_subrot_t *pilha_peek_chamada_subrot(pilha_t *pilha) { 
    item_pilha_t *item = pilha->top;
    if (item) {
        return item->infos_chamada_subrot;
    } else {
        return NULL;
    }
}

// Pilha de tipos

void pilha_push_tipo(pilha_t *pilha, tipo_var tipo) {
    item_pilha_t *item = malloc(sizeof(item_pilha_t));
    item->tipo = tipo;
    item->prev = pilha->top;
    pilha->top = item;
    pilha->length++;
}

tipo_var pilha_pop_tipo(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        pilha->top = item->prev;
        pilha->length--;
        tipo_var tipo = item->tipo;
        free(item);
        return tipo;
    } else {
        return QUALQUER_TIPO;
    }
}

tipo_var pilha_peek_tipo(pilha_t *pilha) {
    item_pilha_t *item = pilha->top;
    if (item) {
        return item->tipo;
    } else {
        return QUALQUER_TIPO;
    }
}