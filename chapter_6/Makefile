EXES = directories files files2 iochannels memprofile timeouts timers
CC = cc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`
GMODULE = `pkg-config --cflags --libs gmodule-2.0`

all: 
	$(MAKE) $(EXES);
	$(MAKE) modules

modules: modules.c modules-plugin.c
	$(CC) -shared modules-plugin.c -o plugin.so $(CFLAGS) $(GMODULE);
	sudo mv plugin.so /usr/lib
	$(CC) $(CFLAGS) $@.c -o $@

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(EXES) modules
