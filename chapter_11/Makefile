CC = cc
CFLAGS = -Wall -g `pkg-config --cflags --libs gtk+-2.0`

all: 
	$(MAKE) ipaddresstest marqueetest

marqueetest: mymarquee.c marqueetest.c mymarquee.h
	$(CC) $(CFLAGS) $@.c mymarquee.c -o $@

ipaddresstest: myipaddress.c ipaddresstest.c myipaddress.h
	$(CC) $(CFLAGS) $@.c myipaddress.c -o $@

myiface:
	make all

clean:
	rm -f marqueetest ipaddresstest
