CONTIKI_PROJECT = compress-disseminate

UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL

APPS += deluge-udp kev-compress

all: $(CONTIKI_PROJECT)

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
