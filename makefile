# $@ : the current target
# $^ : the current prerequisites
# $< : the first current prerequisite

CC=gcc
LDFLAGS=-Wall -ly
REPORTFLAGS= --report=all
BISONFLAGS= -d
EXEC=tpcc


$(EXEC): $(EXEC).tab.o lex.yy.o tree.o table.o traducteur.o
	rm -f _anonymous.o _anonymous utils.o utils
	$(CC) -o $@ $^ $(LDFLAGS)
	mv lex.yy.c "obj/"
	mv *.o "obj/"
	mv *.h "obj/"
	mv $(EXEC) "bin/"

lex.yy.c: src/$(EXEC).lex
	flex $<


%.tab.c: src/%.y
	bison $(REPORTFLAGS) $(BISONFLAGS) $<

%.o: src/%.c
	$(CC) -o $@ -c $<

clean:
	rm -f obj/*

cleanall:
	rm -f obj/*
	rm -f bin/$(EXEC)


#nasm -f elf64 -o _anonymous.o _anonymous.asm
#gcc -o _anonymous _anonymous.o -nostartfiles -no-pie	