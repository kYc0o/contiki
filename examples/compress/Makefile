CONTIKI_PROJECT = compress
CONTIKI_SOURCEFILES += compress.c

UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL

APPS += deluge-udp kev-compress

all: $(CONTIKI_PROJECT)

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
