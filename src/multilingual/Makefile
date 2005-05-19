# Makefile for multilingual speech server
# This is intended for using with Linux and gcc

all: speech_server

clean:
	rm -f speech_server *.o

.o:
	g++ -s -o $@ $^ -lm

.c.o:
	gcc -c -O2 $<

.cc.o:
	g++ -c -O2 $<

# Dependencies
regerror.o: regerror.c
regexp.o: regexp.c regexp.h regmagic.h
regsub.o: regsub.c regexp.h regmagic.h
xmalloc.o: xmalloc.c
English.o: English.cc chain.h substitutor.h regexp.h punctuations.h \
 language.h English.h
Russian.o: Russian.cc chain.h substitutor.h regexp.h punctuations.h \
 language.h Russian.h
chain.o: chain.cc chain.h
language.o: language.cc xmalloc.h chain.h substitutor.h regexp.h \
 punctuations.h language.h regexp.h
punctuations.o: punctuations.cc punctuations.h chain.h substitutor.h \
 regexp.h
queue.o: queue.cc queue.h
source.o: source.cc xmalloc.h source.h
speech_server.o: speech_server.cc xmalloc.h queue.h subprocess.h \
 source.h punctuations.h chain.h language.h English.h Russian.h
subprocess.o: subprocess.cc queue.h language.h chain.h subprocess.h
substitutor.o: substitutor.cc substitutor.h regexp.h chain.h xmalloc.h

speech_server: speech_server.o source.o subprocess.o \
 xmalloc.o regexp.o regsub.o regerror.o \
 chain.o queue.o substitutor.o punctuations.o \
 language.o English.o Russian.o
