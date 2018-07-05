#CFLAGS = -I$(LIBEVENT)/include -Wall
CFLAGS = -Iinclude -Wall
#LIBS =	$(LIBEVENT)/.libs/libevent.a -lrt
LIBS = libevent.a -lrt

http-server: http-server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f http-server *~
