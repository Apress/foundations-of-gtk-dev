EXES_NOGLADE = exercise2-1 exercise2-2 exercise3-1 exercise3-2 exercise4-1 exercise4-2 exercise4-2 exercise5-1 exercise6-1 exercise6-2 exercise7-1 exercise8-1 exercise9-1 exercise9-2
EXES_GLADE = exercise10-1 exercise10-2 exercise12-1
CC = cc
CFLAGS = -Wall -export-dynamic -g `pkg-config --cflags --libs gtk+-2.0 libglade-2.0`

all: 
	$(MAKE) $(EXES_NOGLADE) $(EXES_GLADE) exercise11-1

exercise11-1: exercise11-1.c mymarquee.c mymarquee.h
	$(CC) $(CFLAGS) $@.c mymarquee.c -o $@

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(EXES_GLADE) $(EXES_NOGLADE) exercise11-1
