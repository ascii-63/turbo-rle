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

all: trle-client worker master net-master-receiver net-master-sender

trle-client.o: $(TP)trle-client.c
	$(CC) -O2 $(CFLAGS) $(MARCH) -c $(TP)trle-client.c -o trle-client.o

trle-client: trlec.o trled.o trle-client.o
	$(CC) trle-client.o trlec.o trled.o $(LDFLAGS) -o trle-client

worker.o: $(TP)worker.c
	$(CC) -O2 $(CFLAGS) $(MARCH) -c $(TP)worker.c -o worker.o

worker: trlec.o trled.o worker.o
	$(CC) worker.o trlec.o trled.o $(LDFLAGS) -o worker

master:
	$(CC) $(TP)master.c -o master

net-master-receiver:
	$(CC) $(TP)net-master-receiver.c -o net-master-receiver

net-master-sender:
	$(CC) $(TP)net-master-sender.c -o net-master-sender

.c.o:
	$(CC) -O3 $(CFLAGS) $(MARCH) $< -c -o $@

clean:
	rm *.o
	rm trle-client
	rm worker
	rm master
	rm net-master-receiver
	rm net-master-sender
