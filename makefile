CC=gcc
CFLAGS=-Wall -Wextra

sim: sim.c pager.c clock.c
	$(CC) $(CFLAGS) -o sim sim.c pager.c clock.c

clean:
	rm -f sim