SHELLOBJS = test_shell.o shell.o
CC = gcc
CFLAGS = -g

all: shell

shell: $(SHELLOBJS)
	gcc $^ $(CFLAGS) -o $@ -lm
	rm -f *.o
shell.o: shell.h
test_shell.o: shell.h

clean:
	rm -f *.o
