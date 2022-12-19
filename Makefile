 # -------------------------------------------------------------------
 #            Arquivo: Makefile
 # -------------------------------------------------------------------
 #              Autor: Bruno Müller Junior
 #               Data: 08/2007
 #      Atualizado em: [09/08/2020, 19h:01m]
 #
 # -------------------------------------------------------------------

$DEPURA=1

compilador: pilha.h simbolo.h lex.yy.c compilador.tab.c compilador.o compilador.h
	gcc pilha.c simbolo.c lex.yy.c compilador.tab.c compilador.o -o compilador -ll -ly -lc -g

lex.yy.c: compilador.l compilador.h
	flex compilador.l

compilador.tab.c: compilador.y compilador.h
	bison compilador.y -d -v

compilador.o : compilador.h compiladorF.c
	gcc -c compiladorF.c -o compilador.o -DDEPURA=$(DEPURA)

clean :
	rm -f compilador.tab.* lex.yy.c compilador.o compilador
