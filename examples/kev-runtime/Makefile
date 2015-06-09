CONTIKI_PROJECT = kev-runtime
CONTIKI_SOURCEFILES += kev-runtime.c \
	myencode.c \
	shell_group.c \
	ShellBasedDeployUnitRetriever.c \
	udpComponent.c \
	deluge_group.c \
	NaiveUDPBasedDeployUnitRetriever.c \
	requests.c

ELF_LOADER=1
TARGET=iotlab-m3

APPS += json-from-file kevoree kevoree-runtime deluge-udp

CONSOLE_CLIENT_SRCS = client.c myencode.c

UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL

#linker optimizations
SMALL=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# shell client
CLIENT=client

# a server implementing a software repository
REPOSITORY_SERVER = UDPKevoreeRepo/src/UDPServer
REPOSITORY_SERVER_SRC = UDPKevoreeRepo/src/UDPServer.java

all: $(CONTIKI_PROJECT)

components:
	cd kev-components && $(MAKE)

client: $(CONSOLE_CLIENT_SRCS)
	gcc -std=gnu99 $(CONSOLE_CLIENT_SRCS) -o client -lpthread

repository-server: $(REPOSITORY_SERVER)

server.x:
	cp requests.h server
	cp requests.c server
	cd server && $(MAKE)
	cp server/server server.x

$(REPOSITORY_SERVER): $(REPOSITORY_SERVER_SRC)
	javac $(REPOSITORY_SERVER_SRC)
	
.PHONY: server.x

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
