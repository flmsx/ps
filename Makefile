SHELLOBJS = test_shell.o shell.o
POPSTAROBJS = popstar.o shell.o
FASTOBJS = popstar_alg0.o image.o
CC = gcc
CFLAGS = -g0 -Winline
CPPFLAGS = `GraphicsMagick++-config --cppflags --cxxflags` -fpermissive
LDFLAGS = `GraphicsMagick++-config --ldflags --libs`

all: shell popstar fast

shell: $(SHELLOBJS)
	gcc $^ $(CFLAGS) -o $@ -lm
	
popstar: $(POPSTAROBJS)
	gcc $^ $(CFLAGS) -o $@

fast: $(FASTOBJS)
	g++ $^ $(CPPFLAGS) -o $@ $(LDFLAGS)

shell.o: shell.h
test_shell.o: shell.h
popstar.o: shell.h list.h

clean:
	rm -f *.o
