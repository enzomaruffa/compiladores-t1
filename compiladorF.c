
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
#include "infos_chamada_subrot.h"
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
pilha_t *pilha_chamada_subrot = NULL;

// Global
int rotulos_criados = 0;

// Frame atual
int alocacoes_pendentes = 0;
infos_compilador_t* infos_atuais;

simbolo_t *subrotina_atual; // Função ou procedimento sendo declarado

// Atribuiçao atual
simbolo_t *simbolo_esquerda_atual = NULL;

// Guarda qual função está sendo chamada, ou o identificador em caso de atribuição
char simbolo_salvo[100];

// Caso esteja chamando uma função
int chamando_funcao = 0;

// === PRIVADO ===
void criar_proximo_rotulo(char *rotulo) {
  sprintf(rotulo, "R%02d", rotulos_criados);
  rotulos_criados += 1;
}

void gera_armz(simbolo_t* simbolo) {
  #ifdef DEPURA
  printf("[gera_armz]");
  #endif

  if (simbolo == NULL) {
    char erro[200];
    sprintf(erro, "Variável não declarada: %s", token);
    imprimeErro(erro);
  }

  if (simbolo->categoria == PROCEDIMENTO) {
    char erro[200];
    sprintf(erro, "Procedimento não pode ser usado para armazenar variável: %s", token);
    imprimeErro(erro);
  }

  // #ifdef DEPURA
  printf("[gera_armz] simbolo->categoria: %d, simbolo->nivel_lexico: %d\n", simbolo->categoria, simbolo->nivel_lexico);
  // #endif

  char comando[100];

  if (simbolo->categoria == VARIAVEL_SIMPLES) {
    sprintf(comando, "ARMZ %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
  } else if (simbolo->categoria == PARAMETRO_FORMAL_VALUE) {
    sprintf(comando, "ARMZ %d, %d", simbolo->nivel_lexico, simbolo->parametro.deslocamento);
  } else if (simbolo->categoria == PARAMETRO_FORMAL_REF) {
    sprintf(comando, "ARMI %d, %d", simbolo->nivel_lexico, simbolo->parametro.deslocamento);
  } else if (simbolo->categoria == FUNCAO) { 
    int num_params = proc_get_qtd_param(simbolo);
    sprintf(comando, "ARMZ %d, %d", simbolo->nivel_lexico, -4 - num_params);
  }

  geraCodigo(NULL, comando);
}

infos_compilador_t * criar_infos_compilador() {
  infos_compilador_t *infos = malloc(sizeof(infos_compilador_t));
  infos->nivel_lexico = 0;
  infos->deslocamento = 0;
  return infos;
}

void aumentar_nivel_lexico() {
  #ifdef DEPURA
  printf("[aumentar_nivel_lexico] infos_atuais->nivel_lexico: %d, infos_atuais->deslocamento: %d\n", infos_atuais->nivel_lexico, infos_atuais->deslocamento);
  #endif

  pilha_push_infos(pilha_infos, infos_atuais);

  infos_compilador_t *novas_infos = criar_infos_compilador();
  novas_infos->nivel_lexico = infos_atuais->nivel_lexico + 1;
  novas_infos->deslocamento = 0;

  infos_atuais = novas_infos;
}

void diminuir_nivel_lexico() {
  #ifdef DEPURA
  printf("[diminuir_nivel_lexico] infos_atuais->nivel_lexico: %d, infos_atuais->deslocamento: %d\n", infos_atuais->nivel_lexico, infos_atuais->deslocamento);
  #endif

  infos_compilador_t *infos_anteriores = pilha_pop_infos(pilha_infos);

  #ifdef DEPURA
  printf("[diminuir_nivel_lexico] infos_anteriores->nivel_lexico: %d, infos_anteriores->deslocamento: %d\n", infos_anteriores->nivel_lexico, infos_anteriores->deslocamento);
  #endif

  free(infos_atuais);
  infos_atuais = infos_anteriores;
}

// Gerenciar infos de subrot
void empilhar_infos_chamada_subrot(simbolo_t *subrot) {
  infos_chamada_subrot_t *infos = malloc(sizeof(infos_chamada_subrot_t));
  infos->simbolo = subrot;
  infos->parametro_atual = 0;
  pilha_push_chamada_subrot(pilha_chamada_subrot, infos);
}

void desempilhar_infos_chamada__subrot() {
  infos_chamada_subrot_t *infos = pilha_pop_chamada_subrot(pilha_chamada_subrot);
  free(infos);
}

simbolo_t *get_simbolo_relevante_atual() { 
  #ifdef DEPURA
  printf("[get_simbolo_relevante_atual] chamando_funcao: %d\n", chamando_funcao);
  #endif

  if (chamando_funcao) { 
    #ifdef DEPURA
    printf("[get_simbolo_relevante_atual] buscando simbolo salvo: %s", simbolo_salvo);
    #endif
    return pilha_get_by_id_simbolo(tabela_simbolos, simbolo_salvo);
  }
  return simbolo_esquerda_atual;
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

// === Inicialização
void inicia_vars_compilador() {
  tabela_simbolos = pilha_create();
  pilha_rotulos = pilha_create();
  pilha_subrotinas = pilha_create();
  pilha_infos = pilha_create();
  pilha_chamada_subrot = pilha_create();
  infos_atuais = criar_infos_compilador();
}

// === Criação de simbolos
void registra_var(char* token) {
  #ifdef DEPURA
  printf("[registra_var] %s, %d, %d", token, infos_atuais->nivel_lexico, infos_atuais->deslocamento);
  #endif

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

  #ifdef DEPURA
  printf("[carregar_simbolo]: %s\n", token);
  #endif

  if (simbolo == NULL) {
    char erro[200];
    sprintf(erro, "Variável não declarada: %s", token);
    imprimeErro(erro);
  }

  if (simbolo->categoria == PROCEDIMENTO || simbolo->categoria == FUNCAO) {
    char erro[200];
    sprintf(erro, "Procedimento ou função não pode ser usado como expressão: %s", token);
    imprimeErro(erro);
  }

  #ifdef DEPURA
  printf("[carregar_simbolo]: categoria: %d, nivel lexico: %d, deslocament: %d \n", simbolo->categoria, simbolo->nivel_lexico, simbolo->variavel.deslocamento);
  #endif

  // Caso eu esteja em uma chamada de outra subrotina e quero chamar uma nova como parâmetro, preciso aumentar o numero atual de parametros
  infos_chamada_subrot_t *chamada_atual = pilha_peek_chamada_subrot(pilha_chamada_subrot);
  #ifdef DEPURA
  printf("[carregar_simbolo]: chamada_atual: %p\n", chamada_atual);
  #endif

  if (chamada_atual != NULL) {
    if (simbolo->categoria == PROCEDIMENTO) {
      char erro[200];
      sprintf(erro, "Procedimento não pode ser usado como expressão: %s", token);
      imprimeErro(erro);
    }

    if (chamada_atual->parametro_atual > proc_get_qtd_param(chamada_atual->simbolo)) {
      char erro[200];
      sprintf(erro, "Número de parâmetros excedido: %s", token);
      imprimeErro(erro);
    }

    simbolo_t *parametro = proc_get_param_at(chamada_atual->simbolo, chamada_atual->parametro_atual);
  
    if (simbolo->categoria == VARIAVEL_SIMPLES && parametro->categoria == PARAMETRO_FORMAL_VALUE) { 
      sprintf(comando, "CRVL %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    } else if (simbolo->categoria == VARIAVEL_SIMPLES && parametro->categoria == PARAMETRO_FORMAL_REF) {
      sprintf(comando, "CREN %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);

    } else if (simbolo->categoria == PARAMETRO_FORMAL_VALUE && parametro->categoria == PARAMETRO_FORMAL_VALUE) {
      sprintf(comando, "CRVL %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    } else if (simbolo->categoria == PARAMETRO_FORMAL_VALUE && parametro->categoria == PARAMETRO_FORMAL_REF) {
      sprintf(comando, "CREN %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);

    } else if (simbolo->categoria == PARAMETRO_FORMAL_REF && parametro->categoria == PARAMETRO_FORMAL_VALUE) {
      sprintf(comando, "CRVI %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    } else if (simbolo->categoria == PARAMETRO_FORMAL_REF && parametro->categoria == PARAMETRO_FORMAL_REF) {
      sprintf(comando, "CRVL %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    }

  } else { 
    if (simbolo->categoria == PARAMETRO_FORMAL_VALUE || simbolo->categoria == VARIAVEL_SIMPLES) { 
      sprintf(comando, "CRVL %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    } else if (simbolo->categoria == PARAMETRO_FORMAL_REF) {
      sprintf(comando, "CRVI %d, %d", simbolo->nivel_lexico, simbolo->variavel.deslocamento);
    }
  }

  geraCodigo(NULL, comando);
};

// === Leitura e escrita de símbolos
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

// === Atribuição
void setar_identificador_esquerda(char* token) {
  simbolo_esquerda_atual = pilha_get_by_id_simbolo(tabela_simbolos, token);

  #ifdef DEPURA
  printf("[setar_identificador_esquerda]: %s\n", token);
  #endif

  if (simbolo_esquerda_atual == NULL) {
    char erro[200];
    sprintf(erro, "Símbolo não declarado: %s [setar_identificador_esquerda]", token);
    imprimeErro(erro);
  }
}

void armazenar_valor_identificador_esquerda() {
  char comando[100];

  if (simbolo_esquerda_atual == NULL) {
    char erro[200];
    sprintf(erro, "simbolo_esquerda_atual nulo. Erro interno do compilador. [armazenar_valor_identificador_esquerda]");
    imprimeErro(erro);
  }

  gera_armz(simbolo_esquerda_atual);

  simbolo_esquerda_atual = NULL;
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

// == Blocos
void comecar_bloco() {
  #ifdef DEPURA
  printf("[comecar_bloco]\n");
  #endif

  // Criar rótulo de começo do bloco atual e colocar na pilha
  char *rotuloBloco = malloc(10);
  criar_proximo_rotulo(rotuloBloco);
  pilha_push_label(pilha_rotulos, rotuloBloco);

  // Gerar um DSVS pro rótulo do bloco atual
  char comando[100];
  sprintf(comando, "DSVS %s", rotuloBloco);
  geraCodigo(NULL, comando);
}

void finalizar_bloco() {
  #ifdef DEPURA
  printf("[finalizar_bloco]\n");
  #endif

  // Tirar rótulo do bloco atual da pilha
  char *rotuloBloco = pilha_pop_label(pilha_rotulos);

  // Gerar código ({rótulo do bloco atual}, NADA)
  geraCodigo(rotuloBloco, "NADA");
}

// === Subrotinas
void salvar_simbolo_identificador(char *token) {
  #ifdef DEPURA
  printf("[salvar_simbolo_identificador] '%s'\n", token);
  #endif

  strcpy(simbolo_salvo, token);
}

void carregar_simbolo_salvo() {
  carregar_simbolo(simbolo_salvo);
  strcpy(simbolo_salvo, "");
}

void inicia_registro_subrot() {
  // Incrementa o nivel léxico
  aumentar_nivel_lexico();
}

void registrar_subrot(char* token, categoria_simbolo tipo_subrot) {
  // TODO: Verificar se já existe?
  // TODO: Verificar se é uma palavra reservada?

  #ifdef DEPURA
  printf("[registrar_subrot]: %s\n", token);
  #endif

  char *rotulo = malloc(10);
  criar_proximo_rotulo(rotulo);

  // Criar simbolo
  simbolo_t *subrot = criar_simbolo(token);

  if (tipo_subrot == FUNCAO) {
    subrot->categoria = FUNCAO;
  } else {
    subrot->categoria = PROCEDIMENTO;
  }

  subrot->nivel_lexico = infos_atuais->nivel_lexico;

  subrot->procedimento.deslocamento = 0;
  subrot->procedimento.primeiro_parametro = NULL;

  strcpy(subrot->procedimento.rotulo, rotulo);

  // TODO: Adicionar tipo de retorno na subrotina

  // Adicionar na tabela de símbolos
  pilha_push_simbolo(tabela_simbolos, subrot);

  // Gerar código ({rótulo do subrot}, ENPR {nível léxico do subrot})
  char comando[100];
  sprintf(comando, "ENPR %d", infos_atuais->nivel_lexico);
  geraCodigo(rotulo, comando);

  // Coloca na pilha de subrotinas
  pilha_push_simbolo(pilha_subrotinas, subrot);

  // Fazer para outras subrotinas
  subrotina_atual = subrot;
}

void inicia_chamada_subrot() { 
  simbolo_t *subrot;

  subrot = get_simbolo_relevante_atual();

  if (subrot == NULL) {
    char erro[200];
    sprintf(erro, "Subrotina não declarada. [inicia_chamada_subrot]");
    imprimeErro(erro);
  }

  empilhar_infos_chamada_subrot(subrot);
}

void inicia_chamada_funcao() {
  geraCodigo(NULL, "AMEM 1");
  chamando_funcao = 1;

  // Caso eu esteja em uma chamada de outra subrotina e quero chamar uma nova como parâmetro, preciso aumentar o numero atual de parametros
  infos_chamada_subrot_t *chamada_atual = pilha_peek_chamada_subrot(pilha_chamada_subrot);
  if (chamada_atual != NULL) {
    proximo_parametro_chamada_subrot();
  }

  inicia_chamada_subrot();
}

void registra_parametro(char* token, int por_referencia) {
  #ifdef DEPURA
  printf("[registra_parametro] '%s', %d\n", token, por_referencia);
  #endif

  simbolo_t *parametro = criar_simbolo(token);

  if (por_referencia) {
    parametro->categoria = PARAMETRO_FORMAL_REF;
  } else {
    parametro->categoria = PARAMETRO_FORMAL_VALUE;
  }

  parametro->nivel_lexico = infos_atuais->nivel_lexico;
  parametro->parametro.proximo_parametro = NULL;
  parametro->parametro.deslocamento = 0;

  proc_adiciona_param(subrotina_atual, parametro);
  pilha_push_simbolo(tabela_simbolos, parametro);
}

void finaliza_parametros_subrotina() {
  #ifdef DEPURA
  printf("[finaliza_parametros_subrotina]\n");
  #endif

  int qtd_parametros = proc_get_qtd_param(subrotina_atual);
  subrotina_atual->procedimento.deslocamento = -4 - qtd_parametros;
  for (simbolo_t *p = subrotina_atual->procedimento.primeiro_parametro; 
        p;
        p = p->parametro.proximo_parametro, qtd_parametros--) {
      p->parametro.deslocamento = -3 - qtd_parametros;
  }

  subrotina_atual = NULL;
}

void chamar_subrot() {
  infos_chamada_subrot_t *chamada = pilha_pop_chamada_subrot(pilha_chamada_subrot);
  simbolo_t *subrot = chamada->simbolo;


  // TODO: Verificar se o número de parâmetros está correto, comparando com o topo da pilha de chamada de subrotinas

  if (subrot == NULL) {
    char erro[200];
    sprintf(erro, "Subrotina não declarada. [chamar_subrot]");
    imprimeErro(erro);
  }

  if (subrot->categoria != PROCEDIMENTO && subrot->categoria != FUNCAO) {
  print_simbolo(subrot);
    char erro[200];
    sprintf(erro, "'%s' não é uma subrotina. [chamar_subrot]", subrot->id);
    imprimeErro(erro);
  }

  char comando[100];
  sprintf(comando, "CHPR %s, %d", subrot->procedimento.rotulo, infos_atuais->nivel_lexico);
  geraCodigo(NULL, comando);

  chamando_funcao = 0;
}

void finaliza_subrot() {
  // Desempilhar subrotina
  simbolo_t *subrotina = pilha_pop_simbolo(pilha_subrotinas);

  // Gerar código (RTPR)
  char comando[100];
  sprintf(comando, "RTPR %d, %d", subrotina->nivel_lexico, proc_get_qtd_param(subrotina));
  geraCodigo(NULL, comando);

  // Desempillhar parâmetros da tabela de simbolos
  int qtd_parametros = proc_get_qtd_param(subrotina);
  #ifdef DEPURA
  printf("QTD PARAMETROS: %d\n", qtd_parametros);
  #endif

  for (int i = 0; i < qtd_parametros; i++) {
    pilha_pop_simbolo(tabela_simbolos);
  }

  // Decrementa o nivel léxico
  diminuir_nivel_lexico();
}

void verifica_se_pode_chamar_funcao() { 
  // Caso eu esteja em uma chamada de outra subrotina e quero chamar uma nova como parâmetro, preciso garantir que o parâmetro formal não é por referência
  infos_chamada_subrot_t *chamada_atual = pilha_peek_chamada_subrot(pilha_chamada_subrot);

  if (chamada_atual != NULL) {
    int num_parametro_atual = chamada_atual->parametro_atual;

    simbolo_t *subrot = chamada_atual->simbolo;
    simbolo_t *param = proc_get_param_at(subrot, num_parametro_atual);

    if (param == NULL) {
      char erro[200];
      sprintf(erro, "Número de parâmetros inválido. [verifica_se_pode_chamar_funcao]");
      imprimeErro(erro);
    }

    if (param->categoria == PARAMETRO_FORMAL_REF) {
      char erro[200];
      sprintf(erro, "Não é possível passar uma função como parâmetro por referência. [verifica_se_pode_chamar_funcao]");
      imprimeErro(erro);
    }
  }
}

void proximo_parametro_chamada_subrot() {
  infos_chamada_subrot_t *chamada_atual = pilha_peek_chamada_subrot(pilha_chamada_subrot);

  if (chamada_atual != NULL) {
    chamada_atual->parametro_atual++;
  }
}