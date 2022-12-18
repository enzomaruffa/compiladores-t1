
/* -------------------------------------------------------------------
 *            Aquivo: compilador.c
 * -------------------------------------------------------------------
 *              Autor: Bruno Muller Junior
 *               Data: 08/2007
 *      Atualizado em: [09/08/2020, 19h:01m]
 *
 * -------------------------------------------------------------------
 *
 * Funções auxiliares ao compilador
 *
 * ------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"
#include "pilha.h"
#include "simbolo.h"

/* -------------------------------------------------------------------
 *  variáveis globais
 * ------------------------------------------------------------------- */

simbolos simbolo, relacao;
char token[TAM_TOKEN];

int alocacoes_pendentes = 0;

pilha_t *tabela_simbolos = NULL;

FILE* fp=NULL;
void geraCodigo (char* rot, char* comando) {

  if (fp == NULL) {
    fp = fopen ("MEPA", "w");
  }

  if ( rot == NULL ) {
    fprintf(fp, "     %s\n", comando); fflush(fp);
  } else {
    fprintf(fp, "%s: %s \n", rot, comando); fflush(fp);
  }
}

int imprimeErro ( char* erro ) {
  fprintf (stderr, "Erro na linha %d - %s\n", nl, erro);
  exit(-1);
}

void inicia_vars_compilador() {
  tabela_simbolos = pilha_create();
}

void registra_var(char* token) {
  simbolo_t* simbolo = criar_simbolo(token);
  simbolo->categoria = VARIAVEL_SIMPLES;
  simbolo->nivel_lexico = 0;
  simbolo->variavel.deslocamento = 0;
  pilha_push(tabela_simbolos, simbolo);
}

void incrementa_aloc_pendentes() {
  alocacoes_pendentes++;
}

void alocar_vars_pendentes() {
  char comando[100];
  sprintf(comando, "AMEM %d", alocacoes_pendentes);
  geraCodigo(NULL, comando);

  alocacoes_pendentes = 0;
}

void carregar_constante(char* token) {
  char comando[100];
  sprintf(comando, "CRCT %s", token);
  geraCodigo(NULL, comando);
}

void carregar_simbolo(char* token) {
  char comando[100];
  simbolo_t* simbolo = pilha_get_by_id(tabela_simbolos, token);
  if (simbolo == NULL) {
    char erro[100];
    sprintf(erro, "Variável não declarada: %s", token);
    imprimeErro(erro);
  }

  sprintf(comando, "CRVL %d", simbolo->variavel.deslocamento);
  geraCodigo(NULL, comando);
};
