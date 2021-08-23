APPS	:= client server
CFLAGS	:= -g3

all: $(APPS)

client: client.c tcp_tput.h
server: server.c tcp_tput.h

clean:
	rm $(APPS)