#ifndef INFOS_COMPILADOR_H
#define INFOS_COMPILADOR_H

typedef struct infos_compilador {
  int nivel_lexico;
  int deslocamento;
  int labels;
} infos_compilador_t;

#endif
