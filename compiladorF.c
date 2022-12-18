
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

simbolo_t *simbolo_esquerda_atual = NULL;

FILE* fp=NULL;

// === PRIVADO ===

int get_posicao_memoria_simbolo(simbolo_t *simbolo) {
  return simbolo->variavel.deslocamento;
}

// === PUBLICO ===

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

  sprintf(comando, "CRVL %d", get_posicao_memoria_simbolo(simbolo));
  geraCodigo(NULL, comando);
};

void setar_identificador_esquerda(char* token) {
  simbolo_esquerda_atual = pilha_get_by_id(tabela_simbolos, token);
  if (simbolo_esquerda_atual == NULL) {
    char erro[100];
    sprintf(erro, "Variável não declarada: %s", token);
    imprimeErro(erro);
  }
}

void armazenar_valor_identificador_esquerda() {
  char comando[100];

  if (simbolo_esquerda_atual == NULL) {
    char erro[100];
    sprintf(erro, "simbolo_esquerda_atual nulo. Erro interno do compilador.");
    imprimeErro(erro);
  }

  sprintf(comando, "ARMZ %d", get_posicao_memoria_simbolo(simbolo_esquerda_atual));
  geraCodigo(NULL, comando);

  simbolo_esquerda_atual = NULL;
}
