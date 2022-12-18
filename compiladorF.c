
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
#include "simbolo.h"
#include "infos_compilador.h"
#include "pilha.h"

/* -------------------------------------------------------------------
 *  variáveis globais
 * ------------------------------------------------------------------- */

simbolos simbolo, relacao;
char token[TAM_TOKEN];

FILE* fp=NULL;

// Pilhas
pilha_t *tabela_simbolos = NULL;
pilha_t *pilha_rotulos = NULL;
pilha_t *pilha_subrotinas = NULL;
pilha_t *pilha_infos = NULL;

// Global
int rotulos_criados = 0;

// Frame atual
int alocacoes_pendentes = 0;
infos_compilador_t* infos_atuais;

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

infos_compilador_t * criar_infos_compilador() {
  infos_compilador_t *infos = malloc(sizeof(infos_compilador_t));
  infos->nivel_lexico = 0;
  infos->deslocamento = 0;
  return infos;
}

void aumentar_nivel_lexico() {
  pilha_push_infos(pilha_infos, infos_atuais);

  infos_compilador_t *novas_infos = criar_infos_compilador();
  novas_infos->nivel_lexico = infos_atuais->nivel_lexico + 1;
  novas_infos->deslocamento = 0;

  infos_atuais = novas_infos;
}

void diminuir_nivel_lexico() {
  infos_compilador_t *infos_anteriores = pilha_pop_infos(pilha_infos);
  free(infos_atuais);
  infos_atuais = infos_anteriores;
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
  pilha_subrotinas = pilha_create();
  pilha_infos = pilha_create();
  infos_atuais = criar_infos_compilador();
}

void registra_var(char* token) {
  simbolo_t* simbolo = criar_simbolo(token);
  simbolo->categoria = VARIAVEL_SIMPLES;
  simbolo->nivel_lexico = infos_atuais->nivel_lexico;
  simbolo->variavel.deslocamento = infos_atuais->deslocamento;
  infos_atuais->deslocamento += 1;

  pilha_push_simbolo(tabela_simbolos, simbolo);
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
  if (!infos_atuais->deslocamento) {
    // Não temos nada alocado
    return;
  }

  char comando[100];
  sprintf(comando, "DMEM %d", infos_atuais->deslocamento);
  geraCodigo(NULL, comando);

  // Remover símbolos
  for (int i = 0; i < infos_atuais->deslocamento; i++) {
    pilha_pop_simbolo(tabela_simbolos);
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
  simbolo_t* simbolo = pilha_get_by_id_simbolo(tabela_simbolos, token);

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
  simbolo_t* simbolo = pilha_get_by_id_simbolo(tabela_simbolos, token);

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
  simbolo_esquerda_atual = pilha_get_by_id_simbolo(tabela_simbolos, token);

  #ifdef DEBUG
  printf("[setar_identificador_esquerda]: %s\n", token);
  #endif

  if (simbolo_esquerda_atual == NULL) {
    char erro[100];
    sprintf(erro, "Símbolo não declarado: %s [setar_identificador_esquerda]", token);
    imprimeErro(erro);
  }
}

void armazenar_valor_identificador_esquerda() {
  char comando[100];

  if (simbolo_esquerda_atual == NULL) {
    char erro[100];
    sprintf(erro, "simbolo_esquerda_atual nulo. Erro interno do compilador. [armazenar_valor_identificador_esquerda]");
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

// === If
void avaliar_if() {
  char *rotuloElse = malloc(10);
  criar_proximo_rotulo(rotuloElse);

  char *rotuloFim = malloc(10);
  criar_proximo_rotulo(rotuloFim);

  pilha_push_label(pilha_rotulos, rotuloFim);
  pilha_push_label(pilha_rotulos, rotuloElse);

  char comando[100];
  sprintf(comando, "DSVF %s", rotuloElse);
  geraCodigo(NULL, comando);
}

void finalizar_if() {
  char *rotuloElse = pilha_pop_label(pilha_rotulos);
  char *rotuloFim = pilha_peek_label(pilha_rotulos);

  char comando[100];
  sprintf(comando, "DSVS %s", rotuloFim);
  geraCodigo(NULL, comando);

  geraCodigo(rotuloElse, "NADA");
}

void finalizar_else() {
  char *rotuloFim = pilha_pop_label(pilha_rotulos);
  geraCodigo(rotuloFim, "NADA");
}

// == Procedures
void registrar_procedure(char* token) {
  // TODO: Verificar se já existe?
  // TODO: Verificar se é uma palavra reservada?

  char *rotulo = malloc(10);
  criar_proximo_rotulo(rotulo);

  // Criar simbolo
  simbolo_t *procedure = criar_simbolo(token);
  procedure->categoria = PROCEDIMENTO;
  procedure->nivel_lexico = infos_atuais->nivel_lexico;

  procedure->procedimento.deslocamento = 0;
  procedure->procedimento.primeiro_parametro = NULL;

  strcpy(procedure->procedimento.rotulo, rotulo);

  // Adicionar na tabela de símbolos
  pilha_push_simbolo(tabela_simbolos, procedure);

  // Gerar código ({rótulo do procedure}, ENPR {nível léxico do procedure})
  char comando[100];
  sprintf(comando, "ENPR %d", infos_atuais->nivel_lexico);
  geraCodigo(rotulo, comando);

  // Coloca na pilha de subrotinas
  pilha_push_simbolo(pilha_subrotinas, procedure);
}

void comecar_bloco() {
  // Criar rótulo de começo do bloco atual e colocar na pilha
  char *rotuloBloco = malloc(10);
  criar_proximo_rotulo(rotuloBloco);
  pilha_push_label(pilha_rotulos, rotuloBloco);

  // Gerar um DSVS pro rótulo do bloco atual
  char comando[100];
  sprintf(comando, "DSVS %s", rotuloBloco);
  geraCodigo(NULL, comando);

  // Incrementa o nivel léxico
  aumentar_nivel_lexico();
}

void finalizar_bloco() {
  // Tirar rótulo do bloco atual da pilha
  char *rotuloBloco = pilha_pop_label(pilha_rotulos);

  // Gerar código ({rótulo do bloco atual}, NADA)
  geraCodigo(rotuloBloco, "NADA");

  // Decrementa o nivel léxico
  diminuir_nivel_lexico();
}

void chamar_procedure() {
  // Pegar da tabela de simbolos
  // Ver se é procedure de fato
  // Realizar a chamada
  // Gerar código (CHPR {rótulo do procedure} {nivel lexico do procedure})

  #ifdef DEBUG
  printf("[chamar_procedure] '%s', %d\n", simbolo_esquerda_atual->procedimento.rotulo, nivel_lexico_atual);
  #endif

  if (simbolo_esquerda_atual == NULL) {
    char erro[100];
    sprintf(erro, "Procedure não declarada. [chamar_procedure]");
    imprimeErro(erro);
  }

  if (simbolo_esquerda_atual->categoria != PROCEDIMENTO) {
    char erro[100];
    sprintf(erro, "'%s' não é um procedure. [chamar_procedure]", token);
    imprimeErro(erro);
  }

  char comando[100];
  sprintf(comando, "CHPR %s %d", simbolo_esquerda_atual->procedimento.rotulo, simbolo_esquerda_atual->nivel_lexico);
  geraCodigo(NULL, comando);
}

void voltar_procedure() {
  // Desempilhar subrotina
  simbolo_t *subrotina = pilha_pop_simbolo(pilha_subrotinas);

  // Gerar código (RTPR)
  char comando[100];
  sprintf(comando, "RTPR %d,%d", subrotina->nivel_lexico, proc_get_qtd_param(subrotina));
  geraCodigo(NULL, comando);
}
