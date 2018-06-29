CFLAGS =	-I$(LIBEVENT)/include -Wall

LIBS =		$(LIBEVENT)/.libs/libevent.a -lrt

http-server: http-server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f http-server *~
