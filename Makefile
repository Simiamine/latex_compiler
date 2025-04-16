FLEX  = /opt/homebrew/opt/flex/bin/flex
BISON = /opt/homebrew/opt/bison/bin/bison
CFLAGS = -I/opt/homebrew/opt/flex/include -I/opt/homebrew/opt/bison/include
LDFLAGS = -L/opt/homebrew/opt/flex/lib

all: main

main: algo.tab.c lex.yy.c
	gcc $(CFLAGS) -o main algo.tab.c lex.yy.c -lfl $(LDFLAGS)

algo.tab.c algo.tab.h: algo.y
	$(BISON) -d algo.y

lex.yy.c: algo.l algo.tab.h
	$(FLEX) algo.l

clean:
	rm -f main lex.yy.c algo.tab.c algo.tab.h