CONTIKI_PROJECT = compression
CONTIKI_SOURCEFILES += compression.c

UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL

APPS += deluge-udp

all: $(CONTIKI_PROJECT)

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
