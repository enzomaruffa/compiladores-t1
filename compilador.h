/* -------------------------------------------------------------------
 *            Arquivo: compilador.h
 * -------------------------------------------------------------------
 *              Autor: Bruno Muller Junior
 *               Data: 08/2007
 *      Atualizado em: [09/08/2020, 19h:01m]
 *
 * -------------------------------------------------------------------
 *
 * Tipos, protótipos e variáveis globais do compilador (via extern)
 *
 * ------------------------------------------------------------------- */


#ifndef compilador_h
#define compilador_h

#define TAM_TOKEN 16

typedef enum simbolos
{
  simb_program,
  simb_label,
  simb_type,
  simb_array,
  simb_of,
  simb_var,
  simb_procedure,
  simb_function,
  simb_begin,
  simb_end,
  simb_goto,
  simb_if,
  simb_then,
  simb_else,
  simb_while,
  simb_do,
  simb_mais,
  simb_menos,
  simb_or,
  simb_and,
  simb_vezes,
  simb_div,
  simb_not,
  simb_atribuicao,
  simb_ponto_e_virgula,
  simb_dois_pontos,
  simb_abre_parenteses,
  simb_fecha_parenteses,
  simb_numero,
  simb_ponto,
  simb_virgula,
  simb_identificador,
  simb_igual,
  simb_dif,
  simb_menor,
  simb_maior,
  simb_menor_igual,
  simb_maior_igual,
  simb_read,
  simb_write,
  simb_integer,
  simb_forward,
  simb_writeln,
  simb_string,
} simbolos;

typedef enum {
  VARIAVEL_SIMPLES,
  PARAMETRO_FORMAL_VALUE,
  PARAMETRO_FORMAL_REF,
  PROCEDIMENTO,
  FUNCAO,
  LABEL
} categoria_simbolo;

typedef enum {
  INTEGER,
  BOOLEAN,
  QUALQUER_TIPO
} tipo_var;


/* -------------------------------------------------------------------
 * variáveis globais
 * ------------------------------------------------------------------- */

extern simbolos simbolo, relacao;
extern char token[TAM_TOKEN];
extern int nivel_lexico;
extern int desloc;
extern int nl;


/* -------------------------------------------------------------------
 * prototipos globais
 * ------------------------------------------------------------------- */

void geraCodigo(char *, char *);
int yylex();
void yyerror(const char *s);

int imprimeErro(char* erro);

void inicia_vars_compilador();

void registra_var(char* token);

void incrementa_aloc_pendentes();
void alocar_vars_pendentes();
void desalocar();

void carregar_constante(char* token);
void carregar_simbolo(char* token);

void ler_simbolo(char* token);
void escrever_simbolo(char* token);
void escrever_constante(char* token);

void setar_identificador_esquerda(char* token);
void armazenar_valor_identificador_esquerda();

void salvar_relacao(simbolos relacao_ctx);
void gerar_relacao();

void comecar_while();
void avaliar_while();
void finalizar_while();

void avaliar_if();
void finalizar_if();
void finalizar_else();

void comecar_bloco();
void finalizar_bloco();

void salvar_simbolo_identificador(char* token);
void carregar_simbolo_salvo();

void inicia_registro_subrot();
void registrar_subrot(char* token, categoria_simbolo categoria);
void inicia_chamada_funcao();
void inicia_chamada_subrot();
void registra_parametro(char* token, int por_referencia);
void finaliza_parametros_subrotina();
void chamar_subrot();
void finaliza_subrot();

void verifica_se_pode_chamar_funcao();
void proximo_parametro_chamada_subrot();

void setar_tipo_variavel(tipo_var tipo);
void finaliza_cabecalho_subrot();

void compara_tipos(tipo_var tipo_1, tipo_var tipo_2, tipo_var tipo_resultado);
void empilha_tipo(tipo_var tipo);
void empilha_tipo_token(char* token);
void empilha_tipo_identificador_esquerda();

void registra_goto(char* token);
void comeco_goto(char *token);
void chama_goto(char *token);
#endif
