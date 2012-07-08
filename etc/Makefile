# Makefile
#
# part of the LSD12 compiler
#
# J. Barthelemy, march 2012


all:
	bison -dty lsd12.y
	flex lsd12.l
	gcc -ggdb -c *.c
	gcc -ggdb -o ../lsd12 *.o 
	rm *.o y.tab.c y.tab.h lex.yy.c

clean:
	-rm *.o y.tab.c y.tab.h lex.yy.c 

distclean: clean
	-rm demo

zip:
	zip -r test.zip etc/lsd12.* etc/ast.* etc/check.* etc/pcode.* etc/sym.*
