# Makefile for lisp part of mbrola-server

# what emacs is called on your system
EMACS = emacs
# How to run in batch mode
BATCH = -batch -q -no-site-file
# How to compile 
COMPILE = -f batch-byte-compile

%.elc:  %.el
	$(EMACS) $(BATCH) $(COMPILE) $<

all:mbrola-voices.elc mbrola-css-speech.elc

