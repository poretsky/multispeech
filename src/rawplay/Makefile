# Making Rawplay

# Compiling options
CFLAGS = -O2
LFLAGS = -s
LIBS = -lm

# Installation paths
prefix = /usr/local
bindir = ${prefix}/bin
docdir = ${prefix}/share/doc/rulex

all: rawplay

rawplay: rawplay.o

install: rawplay
	install -D -g root -o root -m 0755 -p $< ${bindir}

clean:
	rm -rf *.o rawplay

%.o:%.c
	gcc -c ${CFLAGS} -o $@ $<

%: %.o
	gcc ${LFLAGS} -o $@ $^ ${LIBS}
