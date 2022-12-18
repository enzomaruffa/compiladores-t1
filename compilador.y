
// Testar se funciona corretamente o empilhamento de par�metros
// passados por valor ou por refer�ncia.


%{
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

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
%%

programa:
   {
      geraCodigo (NULL, "INPP");
   }
   T_PROGRAM T_IDENT
   T_ABRE_PARENTESES lista_idents T_FECHA_PARENTESES T_PONTO_E_VIRGULA
   bloco T_PONTO
   {
      geraCodigo (NULL, "PARA");
   }
;

bloco:
   parte_declara_vars
   { }
   comando_composto { desalocar(); }
;

parte_declara_vars:
   var
;


var:
   { } 
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
   T_IDENT
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

lista_idents:
   lista_idents T_VIRGULA T_IDENT
   | T_IDENT
;


comando_composto:
   T_BEGIN comandos T_END
;

comandos:
   comando T_PONTO_E_VIRGULA comandos
   | comando T_PONTO_E_VIRGULA
   | comando
   |
;

comando:
   T_IDENT { setar_identificador_esquerda(token); } atribuicao
;

atribuicao:
   T_ATRIBUICAO expr { armazenar_valor_identificador_esquerda(); }
;

expr:
   expr_simples
   /* | expr_simples relacao expr_simples */
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
   T_MAIS termo { geraCodigo(NULL, "SOMA"); }
   | T_MENOS termo { geraCodigo(NULL, "SUBT"); }
   | T_OR termo { geraCodigo(NULL, "DISJ"); }
;

termo:
   fator
   | termo op_fator
;

op_fator:
   T_VEZES fator { geraCodigo(NULL, "MULT"); }
   | T_DIV fator { geraCodigo(NULL, "DIVI"); }
   | T_AND fator { geraCodigo(NULL, "CONJ"); }
;

fator:
   T_IDENT { carregar_simbolo(token);}
   | T_NUMERO { carregar_constante(token); }
   | T_ABRE_PARENTESES expr T_FECHA_PARENTESES
   | T_NOT fator
;

relacao:
   T_IGUAL { save_relation(simb_igual); }
   | T_DIF { save_relation(simb_dif); }
   | T_MENOR { save_relation(simb_menor); }
   | T_MAIOR { save_relation(simb_maior); }
   | T_MENOR_IGUAL { save_relation(simb_menor_igual); }
   | T_MAIOR_IGUAL { save_relation(simb_maior_igual); }
;


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
