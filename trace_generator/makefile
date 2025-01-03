## makefile for trace_generator
## Atlantis Young, NUDT
##
CC = g++
LIB = -I./ip-address/include
CFLAGS = -O2 -std=c++11 $(LIB)
.cc.o:
	${CC} ${CFLAGS} -c $*.cc 

db_parser.o : stdinc.h db_parser.h uint128_t.h
dlist.o : stdinc.h dlist.h
FilterList.o : stdinc.h FilterList.h
flist.o : stdinc.h flist.h
hlist.o : stdinc.h hlist.h
trace_generator.o : stdinc.h db_parser.h trace_tools.h FilterList.h
trace_tools.o : stdinc.h dlist.h hlist.h trace_tools.h
uint128_t.o : uint128_t.h

trace_generator: trace_generator.o db_parser.o dlist.o FilterList.o flist.o hlist.o trace_tools.o uint128_t.o
	${CC} ${CFLAGS} trace_generator.o db_parser.o dlist.o FilterList.o flist.o hlist.o trace_tools.o uint128_t.o -o trace_generator

all:	trace_generator

clean:
	rm -rf *.o trace_generator

