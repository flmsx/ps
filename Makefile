SHELLOBJS = test_shell.o shell.o
POPSTAROBJS = popstar.o shell.o

CC = gcc
CFLAGS = -g -Winline

all: shell popstar fast

shell: $(SHELLOBJS)
	gcc $^ $(CFLAGS) -o $@ -lm
	
popstar: $(POPSTAROBJS)
	gcc $^ $(CFLAGS) -o $@

fast: popstar_alg0.cpp
	g++ $^ -o $@ -fpermissive

shell.o: shell.h
test_shell.o: shell.h
popstar.o: shell.h list.h

clean:
	rm -f *.o
