CONTIKI_PROJECT = kev-runtime
CONTIKI_SOURCEFILES += kev-runtime.c components.c

APPS += kevoree-runtime

CONSOLE_CLIENT_SRCS = client.c myencode.c

all: $(CONTIKI_PROJECT)

client: $(CONSOLE_CLIENT_SRCS)
	gcc -std=gnu99 $(CONSOLE_CLIENT_SRCS) -o client -lpthread

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
