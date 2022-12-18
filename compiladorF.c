
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

FILE* fp=NULL;

// Pilhas
pilha_t *tabela_simbolos = NULL;
pilha_t *pilha_rotulos = NULL;

// Global
int rotulos_criados = 0;

// Frame atual
int alocacoes_pendentes = 0;
int nivel_lexico_atual = 0;
int deslocamento_atual = 0;

// Atribuiçao atual
simbolo_t *simbolo_esquerda_atual = NULL;

// === PRIVADO ===

void criar_proximo_rotulo(char *rotulo) {
  sprintf(rotulo, "R%d", rotulos_criados);
  rotulos_criados += 1;
}

void gera_armz(simbolo_t* simbolo) {
  if (simbolo == NULL) {
    char erro[100];
    sprintf(erro, "Variável não declarada: %s", token);
    imprimeErro(erro);
  }

  if (simbolo->categoria == PROCEDIMENTO || simbolo->categoria == FUNCAO) {
    char erro[100];
    sprintf(erro, "Procedimento ou função não pode ser usado para armazenar variável: %s", token);
    imprimeErro(erro);
  }

  char comando[100];
  sprintf(comando, "ARMZ %d,%d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
  geraCodigo(NULL, comando);
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
  pilha_rotulos = pilha_create();
}

void registra_var(char* token) {
  simbolo_t* simbolo = criar_simbolo(token);
  simbolo->categoria = VARIAVEL_SIMPLES;
  simbolo->nivel_lexico = nivel_lexico_atual;
  simbolo->variavel.deslocamento = deslocamento_atual;
  deslocamento_atual += 1;

  pilha_push(tabela_simbolos, simbolo);
}

// === Alocação
void incrementa_aloc_pendentes() {
  alocacoes_pendentes++;
}

void alocar_vars_pendentes() {
  char comando[100];
  sprintf(comando, "AMEM %d", alocacoes_pendentes);
  geraCodigo(NULL, comando);

  alocacoes_pendentes = 0;
}

void desalocar() { 
  if (!deslocamento_atual) {
    // Não temos nada alocado
    return;
  }

  char comando[100];
  sprintf(comando, "DMEM %d", deslocamento_atual);
  geraCodigo(NULL, comando);

  // Remover símbolos
  for (int i = 0; i < deslocamento_atual; i++) {
    pilha_pop(tabela_simbolos);
  }
}

// === Carregamento
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

  if (simbolo->categoria == PROCEDIMENTO || simbolo->categoria == FUNCAO) {
    char erro[100];
    sprintf(erro, "Procedimento ou função não pode ser usado como expressão: %s", token);
    imprimeErro(erro);
  }

  sprintf(comando, "CRVL %d,%d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
  geraCodigo(NULL, comando);
};

void ler_simbolo(char* token) {
  char comando[100];
  simbolo_t* simbolo = pilha_get_by_id(tabela_simbolos, token);

  sprintf(comando, "LEIT");
  geraCodigo(NULL, comando);

  gera_armz(simbolo);
}

void escrever_simbolo(char* token) {
  char comando[100];
  
  carregar_simbolo(token);

  sprintf(comando, "IMPR");
  geraCodigo(NULL, comando);
}

void escrever_constante(char* token) {
  char comando[100];
  
  carregar_constante(token);

  sprintf(comando, "IMPR");
  geraCodigo(NULL, comando);
}

// === Expr
void salvar_relacao(simbolos relacao_ctx) {
  relacao = relacao_ctx;
}

void gerar_relacao() {
    switch (relacao) {
        case simb_igual: geraCodigo(NULL, "CMIG"); break;
        case simb_dif: geraCodigo(NULL, "CMDG"); break;
        case simb_menor: geraCodigo(NULL, "CMME"); break;
        case simb_maior: geraCodigo(NULL, "CMMA"); break;
        case simb_menor_igual: geraCodigo(NULL, "CMEG"); break;
        case simb_maior_igual: geraCodigo(NULL, "CMAG"); break;
        default: break;
    }
}

// === Atribuição
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

  gera_armz(simbolo_esquerda_atual);

  simbolo_esquerda_atual = NULL;
}

// === While
void comecar_while() {
  char *rotuloInicio = malloc(10);
  criar_proximo_rotulo(rotuloInicio);

  char *rotuloFim = malloc(10);
  criar_proximo_rotulo(rotuloFim);

  pilha_push_label(pilha_rotulos, rotuloInicio);
  pilha_push_label(pilha_rotulos, rotuloFim);

  geraCodigo(rotuloInicio, "NADA");
}

void avaliar_while() {
  // TODO: Verificar, se não for booleano imprimir erro
  char *rotuloFim = pilha_peek_label(pilha_rotulos);

  char comando[100];
  sprintf(comando, "DSVF %s", rotuloFim);
  geraCodigo(NULL, comando);
}

void finalizar_while() {
  char *rotuloFim = pilha_pop_label(pilha_rotulos);
  char *rotuloInicio = pilha_pop_label(pilha_rotulos);

  char comando[100];
  sprintf(comando, "DSVS %s", rotuloInicio);
  geraCodigo(NULL, comando);

  geraCodigo(rotuloFim, "NADA");
}
