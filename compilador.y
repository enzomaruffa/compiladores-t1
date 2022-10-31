
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
%token T_IDENT

%%

programa:   {
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
      {

      }
      comando_composto
;

parte_declara_vars:  var
;


var:  { } T_VAR declara_vars
      |
;

declara_vars: declara_vars declara_var
            | declara_var
;

declara_var:   { }
               lista_id_var T_DOIS_PONTOS
               tipo
               {
                  /* AMEM */
               }
               T_PONTO_E_VIRGULA
;

tipo: T_IDENT
;

lista_id_var: lista_id_var T_VIRGULA T_IDENT
              { /* insere �ltima vars na tabela de s�mbolos */ }
            | T_IDENT { /* insere vars na tabela de s�mbolos */}
;

lista_idents: lista_idents T_VIRGULA T_IDENT
            | T_IDENT
;


comando_composto: T_BEGIN comandos T_END
;

comandos: expr
;

expr: expr T_MAIS termo {printf ("+"); }
    | expr T_MENOS termo {printf ("-"); }
    | termo
;

termo: termo T_VEZES fator  {printf ("*");}|
     | termo T_DIV fator  {printf ("/"); }
     | fator
;

fator      : T_IDENT {printf ("A"); }
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

   yyin=fp;
   yyparse();

   return 0;
}
