CC = gcc

CFLAGS = -lm -I./include/

OFLAGS = -O2

GFLAGS = -g -Wall

SOURCES = ./src/*.c

all: markov

markov: ctags
	$(CC) $(SOURCES) $(CFLAGS) $(GFLAGS) -o markov 

run:
	./markov

clean:
	rm markov

fresh: clean markov

optimized:
	$(CC) $(SOURCES) $(CFLAGS) $(OFLAGS) -o markov

ctags: 
	ctags -R *
