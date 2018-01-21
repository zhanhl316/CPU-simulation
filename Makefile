all: msh
msh: main.o lex.yy.o
	gcc -o msh main.o lex.yy.o -lfl  
main.o: main.c
	gcc -Wall -g -c main.c
lex.yy.o: lex.yy.c
	gcc -Wall -g -c lex.yy.c
lex.yy.c: lex.c
	flex lex.c
clean:
	rm -f *.o
	rm -f lex.yy.c
	rm -f msh
	