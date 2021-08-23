APPS	:= client server

all: $(APPS)

client: client.c
server: server.c

clean:
	rm $(APPS)