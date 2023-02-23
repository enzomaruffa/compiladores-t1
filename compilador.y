
// Testar se funciona corretamente o empilhamento de par�metros
// passados por valor ou por refer�ncia.


%{
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"
#include "simbolo.h"

int num_vars;

%}

%token T_PROGRAM
%token T_LABEL
%token T_TYPE
%token T_ARRAY
%token T_OF
%token T_VAR
%token T_PROCEDURE
%token T_FUNCTION
%token T_BEGIN
%token T_END
%token T_GOTO
%token T_IF
%token T_THEN
%token T_ELSE
%token T_WHILE
%token T_DO
%token T_MAIS
%token T_MENOS
%token T_OR
%token T_AND
%token T_VEZES
%token T_DIV
%token T_NOT
%token T_ATRIBUICAO
%token T_PONTO_E_VIRGULA
%token T_DOIS_PONTOS
%token T_VIRGULA
%token T_PONTO
%token T_ABRE_PARENTESES
%token T_FECHA_PARENTESES
%token T_NUMERO
%token T_IDENT
%token T_IGUAL
%token T_DIF
%token T_MENOR
%token T_MAIOR
%token T_MENOR_IGUAL
%token T_MAIOR_IGUAL
%token T_READ
%token T_WRITE
%token T_INTEGER

%nonassoc T_LOWER_THAN_ELSE
%nonassoc T_ELSE

%%

programa:
   {
      geraCodigo (NULL, "INPP");
   }
   T_PROGRAM T_IDENT
   parametros_programa T_PONTO_E_VIRGULA
   bloco T_PONTO
   {
      geraCodigo (NULL, "PARA");
   }
;

parametros_programa:
   T_ABRE_PARENTESES lista_idents T_FECHA_PARENTESES
   |

lista_idents:
   lista_idents T_VIRGULA T_IDENT
   | T_IDENT
;

bloco:
   parte_declara_gotos
   parte_declara_vars
   { comecar_bloco();}
   parte_declara_subrots
   { finalizar_bloco(); }
   comando_composto
   { desalocar(); }
;

parte_declara_vars:
   var
;

var:
   T_VAR declara_vars
   |
;

declara_vars:
   declara_vars declara_var
   | declara_var
;

declara_var:
   { }
   lista_id_var T_DOIS_PONTOS
   tipo
   {
      /* AMEM */
      alocar_vars_pendentes();
   }
   T_PONTO_E_VIRGULA
;

tipo:
   T_INTEGER { setar_tipo_variavel(INTEGER); }
;

lista_id_var:
   lista_id_var T_VIRGULA T_IDENT {
      registra_var(token);
      incrementa_aloc_pendentes();
   }
   | T_IDENT {
      registra_var(token);
      incrementa_aloc_pendentes();
   }
;

parte_declara_gotos:
   goto
;

goto:
   T_LABEL declara_gotos
   |
;

declara_gotos:
   declara_gotos declara_goto
   | declara_goto
;

declara_goto:
   lista_id_goto T_PONTO_E_VIRGULA
;

lista_id_goto:
   lista_id_goto T_VIRGULA T_NUMERO {
      registra_goto(token);
   }
   | T_NUMERO {
      registra_goto(token);
   }
;

parte_declara_subrots:
   parte_declara_subrots declara_procedure T_PONTO_E_VIRGULA 
   | parte_declara_subrots declara_funcao T_PONTO_E_VIRGULA
   |

declara_funcao:
   T_FUNCTION { inicia_registro_subrot(); } T_IDENT { registrar_subrot(token, FUNCAO); }
   parametros_formais_subrot
   { finaliza_parametros_subrotina(); }
   T_DOIS_PONTOS tipo 
   { finaliza_cabecalho_subrot(); }
   T_PONTO_E_VIRGULA 
   bloco
   { finaliza_subrot(); }

declara_procedure:
   T_PROCEDURE { inicia_registro_subrot(); } T_IDENT { registrar_subrot(token, PROCEDIMENTO); }
   parametros_formais_subrot
   { finaliza_parametros_subrotina(); finaliza_cabecalho_subrot(); }
   T_PONTO_E_VIRGULA
   bloco
   { finaliza_subrot(); }

parametros_formais_subrot:
   T_ABRE_PARENTESES params_formais_rep T_FECHA_PARENTESES
   |

params_formais_rep:
    params_formais_rep T_PONTO_E_VIRGULA secao_de_params_formais
    | secao_de_params_formais
;

secao_de_params_formais:
    T_VAR lista_params_ref T_DOIS_PONTOS tipo
    | lista_params_val T_DOIS_PONTOS tipo
;

lista_params_ref:
    lista_params_ref T_VIRGULA T_IDENT { registra_parametro(token, 1); }
    | T_IDENT { registra_parametro(token, 1); }
;

lista_params_val:
    lista_params_val T_VIRGULA T_IDENT { registra_parametro(token, 0); }
    | T_IDENT { registra_parametro(token, 0); }
;

comando_ou_composto:
   comando
   | comando_composto

comando_composto:
   T_BEGIN comandos T_END
;

comandos:
   comando T_PONTO_E_VIRGULA comandos
   | comando T_PONTO_E_VIRGULA
   | comando
   | comeco_goto comandos
;

comando:
   read
   | write
   | while
   | if
   | comando_identificador_esquerda
   | chama_goto
;

comando_identificador_esquerda:
   T_IDENT { setar_identificador_esquerda(token); }
   comando_identificador_esquerda_fim

comando_identificador_esquerda_fim:
   atribuicao
   | chamada_procedure

atribuicao:
   T_ATRIBUICAO expr { 
      empilha_tipo_identificador_esquerda(); 
      compara_tipos(QUALQUER_TIPO, QUALQUER_TIPO, QUALQUER_TIPO); 
      armazenar_valor_identificador_esquerda(); 
   }

chamada_procedure:
   T_ABRE_PARENTESES { inicia_chamada_subrot(); } lista_parametros_reais T_FECHA_PARENTESES
   { chamar_subrot(); }
   | { chamar_subrot(); }

lista_parametros_reais:
   lista_parametros_reais T_VIRGULA expr { proximo_parametro_chamada_subrot(); }
   | expr { proximo_parametro_chamada_subrot(); }
   |


read:
   T_READ T_ABRE_PARENTESES lista_read T_FECHA_PARENTESES

lista_read:
   lista_read T_VIRGULA T_IDENT { ler_simbolo(token); }
   | T_IDENT { ler_simbolo(token); }

write:
   T_WRITE T_ABRE_PARENTESES lista_write T_FECHA_PARENTESES

lista_write:
   lista_write T_VIRGULA T_IDENT { escrever_simbolo(token); }
   | T_IDENT { escrever_simbolo(token); }
   | lista_write T_VIRGULA T_NUMERO { escrever_constante(token); }
   | T_NUMERO { escrever_constante(token); }

while:
   T_WHILE { comecar_while(); }
   expr { 
      empilha_tipo(BOOLEAN); 
      compara_tipos(BOOLEAN, BOOLEAN, QUALQUER_TIPO); 
      avaliar_while(); }
   T_DO comando_composto { finalizar_while(); }

if:
   T_IF expr { 
      empilha_tipo(BOOLEAN); 
      compara_tipos(BOOLEAN, BOOLEAN, QUALQUER_TIPO); 
      avaliar_if(); 
   }
   T_THEN comando_ou_composto { finalizar_if(); }
   else { finalizar_else(); }

else:
   T_ELSE comando_ou_composto
   | %prec T_LOWER_THAN_ELSE

expr:
   expr_simples
   | expr_simples relacao expr_simples { compara_tipos(QUALQUER_TIPO, QUALQUER_TIPO, BOOLEAN); gerar_relacao(); }
;

expr_simples:
   termo_com_sinal lista_op_termos
;

termo_com_sinal:
   T_MAIS termo
   | T_MENOS termo 
   | termo
;

lista_op_termos:
   lista_op_termos op_termo
   | op_termo
   |
;

op_termo:
   T_MAIS termo { geraCodigo(NULL, "SOMA"); compara_tipos(INTEGER, INTEGER, INTEGER); }
   | T_MENOS termo { geraCodigo(NULL, "SUBT"); compara_tipos(INTEGER, INTEGER, INTEGER); }
   | T_OR termo { geraCodigo(NULL, "DISJ"); compara_tipos(BOOLEAN, BOOLEAN, BOOLEAN); }
;

termo:
   fator
   | termo op_fator
;

op_fator:
   T_VEZES fator { geraCodigo(NULL, "MULT"); compara_tipos(INTEGER, INTEGER, INTEGER); }
   | T_DIV fator { geraCodigo(NULL, "DIVI"); compara_tipos(INTEGER, INTEGER, INTEGER); }
   | T_AND fator { geraCodigo(NULL, "CONJ"); compara_tipos(BOOLEAN, BOOLEAN, BOOLEAN); }
;

fator:
   T_IDENT { salvar_simbolo_identificador(token); empilha_tipo_token(token); } var_ou_chama_funcao
   | T_NUMERO { carregar_constante(token); empilha_tipo(INTEGER); }
   | T_ABRE_PARENTESES expr T_FECHA_PARENTESES
   | T_NOT fator { empilha_tipo(BOOLEAN); }
;

relacao:
   T_IGUAL { salvar_relacao(simb_igual); }
   | T_DIF { salvar_relacao(simb_dif); }
   | T_MENOR { salvar_relacao(simb_menor); }
   | T_MAIOR { salvar_relacao(simb_maior); }
   | T_MENOR_IGUAL { salvar_relacao(simb_menor_igual); }
   | T_MAIOR_IGUAL { salvar_relacao(simb_maior_igual); }
;

var_ou_chama_funcao:
   T_ABRE_PARENTESES { verifica_se_pode_chamar_funcao(); inicia_chamada_funcao(); } lista_parametros_reais T_FECHA_PARENTESES
   { chamar_subrot(); }
   | { carregar_simbolo_salvo(); }

comeco_goto:
   T_NUMERO { comeco_goto(token); } T_DOIS_PONTOS

chama_goto:
   T_GOTO T_NUMERO { chama_goto(token); }

%%

int main (int argc, char** argv) {
   FILE* fp;
   extern FILE* yyin;

   if (argc<2 || argc>2) {
         printf("usage compilador <arq>a %d\n", argc);
         return(-1);
      }

   fp=fopen (argv[1], "r");
   if (fp == NULL) {
      printf("usage compilador <arq>b\n");
      return(-1);
   }


/* -------------------------------------------------------------------
 *  Inicia a Tabela de S�mbolos
 * ------------------------------------------------------------------- */
   inicia_vars_compilador();

   yyin=fp;
   yyparse();

   return 0;
}
