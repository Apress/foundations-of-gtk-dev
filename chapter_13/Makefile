CC = cc
CFLAGS = -Wall -g -export-dynamic `pkg-config --cflags --libs gtk+-2.0 libglade-2.0`

all: 
	$(MAKE) browser calculator ping calendar hangman

browser: browser.c navigation.c
	$(CC) $(CFLAGS) $@.c navigation.c -o $@

calculator: calculator.c
	$(CC) $(CFLAGS) $@.c -o $@

ping: ping.c
	$(CC) $(CFLAGS) $@.c -o $@

calendar: calendar.c
	$(CC) $(CFLAGS) $@.c -o $@

hangman: hangman.c
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -f browser calculator ping calendar hangman
