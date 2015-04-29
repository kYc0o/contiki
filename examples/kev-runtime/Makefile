CONTIKI_PROJECT = kev-runtime
CONTIKI_SOURCEFILES += kev-runtime.c components.c myencode.c

ELF_LOADER=1

APPS += kevoree-runtime

CONSOLE_CLIENT_SRCS = client.c myencode.c

all: $(CONTIKI_PROJECT)

client: $(CONSOLE_CLIENT_SRCS)
	gcc -std=gnu99 $(CONSOLE_CLIENT_SRCS) -o client -lpthread

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
