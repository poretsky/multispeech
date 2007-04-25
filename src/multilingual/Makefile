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
	g++ -c -Wall -O2 $<

# Dependencies
xmalloc.o: xmalloc.c
Brazilian.o: Brazilian.cc chain.h substitutor.h punctuations.h \
 language.h Brazilian.h expansion.h
English.o: English.cc chain.h substitutor.h punctuations.h \
 language.h English.h
French.o: French.cc chain.h substitutor.h punctuations.h \
 language.h French.h
German.o: German.cc chain.h substitutor.h punctuations.h \
 language.h German.h
Russian.o: Russian.cc chain.h substitutor.h punctuations.h \
 language.h Russian.h
Spanish.o: Spanish.cc chain.h substitutor.h punctuations.h \
 language.h Spanish.h
chain.o: chain.cc chain.h
expansion.o: expansion.cc expansion.h
language.o: language.cc xmalloc.h chain.h substitutor.h \
 punctuations.h language.h
punctuations.o: punctuations.cc punctuations.h chain.h substitutor.h
queue.o: queue.cc queue.h
source.o: source.cc xmalloc.h source.h
speech_server.o: speech_server.cc xmalloc.h queue.h subprocess.h \
 source.h punctuations.h chain.h language.h Brazilian.h English.h French.h German.h Russian.h Spanish.h
subprocess.o: subprocess.cc queue.h language.h chain.h subprocess.h
substitutor.o: substitutor.cc substitutor.h chain.h xmalloc.h

speech_server: speech_server.o source.o subprocess.o \
 xmalloc.o chain.o queue.o substitutor.o punctuations.o \
 language.o Brazilian.o English.o French.o German.o Russian.o expansion.o Spanish.o
