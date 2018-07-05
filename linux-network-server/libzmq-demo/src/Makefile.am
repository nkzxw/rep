AUTOMAKE_OPTIONS=foreign
INCLUDES=-I../include 
#main_LDADD = -lzmq 
#$(top_builddir)/libbwfsapi/libbwfsapi.la
#server_LDFLAGS = -pthread -lrt
#main_LDADD = /root/libzmq/.libs/libzmq.a 
#main_LDADD = /usr/local/lib/libzmq.la 
#main_LDADD = /root/libzmq//libzmq.la 
#main_SOURCES = test_bind_src_address.cpp
noinst_PROGRAMS = server client  #main  
server_SOURCES = zmq_server.cpp basetypes.h common.h debuglog.c debuglog.h 
client_SOURCES = zmq_client.cpp basetypes.h common.h debuglog.c debuglog.h 
LDADD = ../libzmq/libzmq.a 
LDFLAGS = -pthread -lrt

