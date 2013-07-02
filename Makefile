SHELLOBJS = test_shell.o shell.o
POPSTAROBJS = popstar.o shell.o

CC = gcc
CFLAGS = -g

all: shell popstar

shell: $(SHELLOBJS)
	gcc $^ $(CFLAGS) -o $@ -lm
	
popstar: $(POPSTAROBJS)
	gcc $^ $(CFLAGS) -o $@

shell.o: shell.h
test_shell.o: shell.h
popstar.o: shell.h list.h

clean:
	rm -f *.o
