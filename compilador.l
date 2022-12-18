
/* -------------------------------------------------------------------
 *            Arquivo: compilador.l
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

%{


#include<stdio.h>
#include<string.h>

#include "compilador.h"
#include "compilador.tab.h"

//#define IMPRIME(STR) {};
#define IMPRIME(STR) printf("%s    # %s \n", STR, token);

int nl = 1;

%}

pulo_linha [\n]

descarte [\t ]+

ident [a-zA-Z][a-zA-Z0-9]*

numero [0-9]+

comentario "(*".*"*)"

%%

program {
    simbolo = simb_program;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("program ");
    return T_PROGRAM;
 }

label {
    simbolo = simb_label;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("label ");
    return T_LABEL;
 }

type {
    simbolo = simb_type;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("type ");
    return T_TYPE;
 }

array {
    simbolo = simb_array;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("array ");
    return T_ARRAY;
 }

of { 
    simbolo = simb_of;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("of ");
    return T_OF;
 }

var {
    simbolo = simb_var;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("var  ");
    return T_VAR;
 }

 procedure {
    simbolo = simb_procedure;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("procedure ");
    return T_PROCEDURE;
 }

function {
    simbolo = simb_function;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("function ");
    return T_FUNCTION;
 }

begin {
    simbolo = simb_begin;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("begin ");
    return T_BEGIN;
 }

end {
    simbolo = simb_end;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("end  ");
    return T_END;
 }

goto {
    simbolo = simb_goto;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("goto ");
    return T_GOTO;
 }

if {
    simbolo = simb_if;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("if   ");
    return T_IF;
 }

then {
    simbolo = simb_then;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("then ");
    return T_THEN;
 }

else {
    simbolo = simb_else;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("else ");
    return T_ELSE;
 }

while {
    simbolo = simb_while;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("while");
    return T_WHILE;
 }

do {
    simbolo = simb_do;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("do   ");
    return T_DO;
 }

\+ {
    simbolo = simb_mais;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("+    ");
    return T_MAIS;
 }


\- {
    simbolo = simb_menos;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("-    ");
    return T_MENOS;
 }

or {
    simbolo = simb_or;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("or   ");
    return T_OR;
 }

and {
    simbolo = simb_and;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("and  ");
    return T_AND;
 }

\* {
    simbolo = simb_vezes;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("*    ");
    return T_VEZES;
 }

div {
    simbolo = simb_div;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("div  ");
    return T_DIV;
 }

not {
    simbolo = simb_not;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME("not  ");
    return T_NOT;
 }

:= {
    simbolo = simb_atribuicao;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" atribuicao  ");
    return T_ATRIBUICAO;
 }

\; {
    simbolo = simb_ponto_e_virgula;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" pt_virgula  ");
    return T_PONTO_E_VIRGULA;
 }

\: {
    simbolo = simb_dois_pontos;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" dois pontos ");
    return T_DOIS_PONTOS;
 }

\, {
    simbolo = simb_virgula;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" virgula  ");
    return T_VIRGULA;
 }


\. {
    simbolo = simb_ponto;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" ponto  ");
    return T_PONTO;
 }

\( {
    simbolo = simb_abre_parenteses;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" abre_parenteses  ");
    return T_ABRE_PARENTESES;
 }

\) {
    simbolo = simb_fecha_parenteses;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" fecha_parenteses ");
    return T_FECHA_PARENTESES;
 }

\= {
    simbolo = simb_igual;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" igual  ");
    return T_IGUAL;
 }

\<> {
    simbolo = simb_dif;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" dif  ");
    return T_DIF;
 }

\<= {
    simbolo = simb_menor_igual;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" menor_igual  ");
    return T_MENOR_IGUAL;
 }

\>= {
    simbolo = simb_maior_igual;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" maior_igual  ");
    return T_MAIOR_IGUAL;
 }

\< {
    simbolo = simb_menor;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" menor  ");
    return T_MENOR;
 }

\> {
    simbolo = simb_maior;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" maior  ");
    return T_MAIOR;
 }

read {
    simbolo = simb_read;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" read  ");
    return T_READ;
 }

write {
    simbolo = simb_write;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" write  ");
    return T_WRITE;
 }

{pulo_linha} {nl++;}

{descarte}

{comentario}

{numero} { 
    simbolo = simb_numero;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" numero ");
    return T_NUMERO;
 }


{ident} {
    simbolo = simb_identificador;
    strncpy (token, yytext, TAM_TOKEN);
    IMPRIME(" ident  ");
    return T_IDENT;
 }

%%
