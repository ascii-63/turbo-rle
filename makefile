CC ?= gcc
CXX ?= g++

DEBUG=-DNDEBUG -s

CFLAGS=$(DEBUG)
#------------------- OS/ARCH -------------------#
CC=gcc
CXX=g++
ARCH=x86_64

CFLAGS=-march=native

MSSE=-mssse3

DEFS+=-falign-loops

LDFLAGS+=-lrt

ifeq ($(STATIC),1)
  LDFLAGS+=-static
endif

ifeq ($(AVX2),1)
  MARCH+=-mbmi2 -mavx2
else
  AVX2=0
endif

CFLAGS+=$(DDEBUG) -w -Wall -std=gnu99 -DUSE_THREADS  -fstrict-aliasing -Iext $(DEFS)
CXXFLAGS+=$(DDEBUG) -w -fpermissive -Wall -fno-rtti -Iext/FastPFor/headers $(DEFS)

#------------------- MAKE -------------------#

all: trle-client

trle-client.o: $(TP)trle-client.c
	$(CC) -O2 $(CFLAGS) $(MARCH) -c $(TP)trle-client.c -o trle-client.o

trle-client: trlec.o trled.o trle-client.o
	$(CC) trle-client.o trlec.o trled.o $(LDFLAGS) -o trle-client
 
.c.o:
	$(CC) -O3 $(CFLAGS) $(MARCH) $< -c -o $@

clean:
	rm *.o
	rm trle-client 
