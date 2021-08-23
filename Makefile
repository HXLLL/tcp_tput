APPS	:= client server
CFLAGS	:= -g3
LD		:= gcc
LDFLAGS	:= ${LDFLAGS} -lpthread

all: $(APPS)

client: client.c tcp_tput.h
	$(LD) $(CFLAGS) -o $@ $^ $(LDFLAGS)
server: server.c tcp_tput.h
	$(LD) $(CFLAGS) -o $@ $^ $(LDFLAGS)
clean:
	rm $(APPS)