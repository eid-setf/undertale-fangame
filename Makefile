##
# Undertale fangame
#
# @file
# @version 0.1

CC=gcc
# CFLAGS=-Wall -g -std=c11 -lraylib -lgdi32 -lwinmm -lm
CFLAGS=-Wall -fms-extensions -g -std=c11 -lraylib -lm
OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
DEPS = $(wildcard *.h)

all: prog

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prog: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	# windows
	# rm -Path . -Include *.o -Include *.exe
	rm *.o *.exe prog

# end
