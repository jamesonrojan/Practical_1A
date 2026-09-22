# Task 1: golden measure on the PC.
CC = gcc
CFLAGS = -O2 -Wall -Wextra

all: golden_measure

golden_measure: golden_measure.c
	$(CC) $(CFLAGS) -o $@ golden_measure.c -lm

run: golden_measure
	./golden_measure

check: run

clean:
	rm -f golden_measure

.PHONY: all run check clean