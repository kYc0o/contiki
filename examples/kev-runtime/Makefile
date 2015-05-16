CONTIKI_PROJECT = kev-runtime
CONTIKI_SOURCEFILES += kev-runtime.c myencode.c shell_group.c ShellBasedDeployUnitRetriever.c udpComponent.c 
#deluge_rime_group.c

ELF_LOADER=1
TARGET=iotlab-m3

APPS += json-from-file kevoree kevoree-runtime
#deluge-kevoree

CONSOLE_CLIENT_SRCS = client.c myencode.c

UIP_CONF_IPV6=1
#CFLAGS+= -DUIP_CONF_IPV6_RPL

#linker optimizations
SMALL=1

#CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

all: $(CONTIKI_PROJECT)

components:
	cd kev-components && $(MAKE)

client: $(CONSOLE_CLIENT_SRCS)
	gcc -std=gnu99 $(CONSOLE_CLIENT_SRCS) -o client -lpthread

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
