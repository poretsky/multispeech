#
# Makefile for jj's siggen  .....

SHELL		= /bin/sh


PROGS		= tones

#
TEXTS		= tones.txt

#
TONES		= tones.o tonesgen.o generator.o misc.o file.o DAC.o

srcdir		= .
includedir	= /usr/include/ncurses
INSDIR		= /usr/local/bin
MANDIR = /usr/local/man/man1
LOCALINS	= $(HOME)/bin
LOCALMAN	= $(HOME)/man/man1

CC		= gcc
CFLAGS		=  -O2  
CPPFLAGS	= -I. -I$(includedir)

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= $(CC)
LDFLAGS		= -lncurses -lgpm -lm

.c.o:
	$(CC) -c $(CCFLAGS) $<

all:	$(PROGS) $(TEXTS) 

text:	$(TEXTS)

%.txt:	%.1
	nroff -man $< | col -b -x > $@


install:
	cp ./tones ../../bin
	cp tones.1 $(MANDIR)

tones:	$(TONES)
	$(CC) -s $(TONES) -lm -o $@

clean:
	@echo cleaning up object and backup files.
	@rm -f *.o $(PROGS) $(TEXTS) *~ 
	@echo removing tones binary.
	@rm -f ../../bin/tones
	@echo removing manual pages.
	@rm -f $(MANDIR)/tones.1
	@echo done.

