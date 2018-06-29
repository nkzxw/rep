A simple http server using libevent.
-------------------------------------------------------------------------------

Requires libevent 2.0+.

First you must download and build libevent, you do not need to install
it.  

    libevent homepage: http://www.monkey.org/~provos/libevent/

To build http-server:

    LIBEVENT=~/src/libevent-2.0.12-stable make

where LIBEVENT points to the location of your built libevent.
