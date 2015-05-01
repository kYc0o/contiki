CONTIKI_PROJECT = kev-runtime
CONTIKI_SOURCEFILES += kev-runtime.c myencode.c shell_group.c ShellBasedDeployUnitRetriever.c

ELF_LOADER=1
TARGET=iotlab-m3

APPS += json kevoree kevoree-runtime

CONSOLE_CLIENT_SRCS = client.c myencode.c

all: $(CONTIKI_PROJECT)

components:
	cd kev-components && $(MAKE)

client: $(CONSOLE_CLIENT_SRCS)
	gcc -std=gnu99 $(CONSOLE_CLIENT_SRCS) -o client -lpthread

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
