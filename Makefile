APPS	:= client server
CFLAGS	:= -g3

all: $(APPS)

client: client.c
server: server.c

clean:
	rm $(APPS)