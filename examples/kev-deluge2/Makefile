CONTIKI_PROJECT = kev-deluge
all: $(CONTIKI_PROJECT)

#UIP_CONF_TCP=1
UIP_CONF_IPV6=1

CFLAGS+= -DUIP_CONF_IPV6_RPL

TARGET=iotlab-m3

APPS+= deluge-udp

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
