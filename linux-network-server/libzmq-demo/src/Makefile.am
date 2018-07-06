AUTOMAKE_OPTIONS=foreign
INCLUDES=-I../include 
#main_LDADD = -lzmq 
#$(top_builddir)/libbwfsapi/libbwfsapi.la
#server_LDFLAGS = -pthread -lrt
#main_LDADD = /root/libzmq/.libs/libzmq.a 
#main_LDADD = /usr/local/lib/libzmq.la 
#main_LDADD = /root/libzmq//libzmq.la 
noinst_PROGRAMS = server client  main  
main_SOURCES = test_bind_src_address.cpp
server_SOURCES = zmq_server.cpp basetypes.h common.cpp common.h debuglog.c debuglog.h 
client_SOURCES = zmq_client.cpp basetypes.h common.cpp common.h debuglog.c debuglog.h 
LDADD = ../libzmq/libzmq.a 
LDFLAGS = -pthread -lrt

