# This is a Makefile for the all this stuf
# You should glance at the scripts subdirectory
# and make sure there is all right. :-)
#
# binaries.
BINS = src/freephone/freephone src/multilingual/speech_server src/tones/tones src/rawplay/rawplay
#
# documentation files.
DOCS = src/tones/tones.1

all: speech_server rawplay freephone tones lisp letters
	mkdir binaries
	cp $(BINS) binaries
	mkdir docs
	cp $(DOCS) docs

# Build speech_server
speech_server:
	cd src/multilingual; $(MAKE) all

# build our modified version of freephone
freephone:
	cd src/freephone; $(MAKE) all

# build rawplay from rawplay.c
rawplay:
	cd src/rawplay; $(MAKE)

# build the tones prg
tones:
	cd src/tones; $(MAKE) all

# Compiling lisp files
lisp:
	cd lisp; $(MAKE) all

# generates the pre-synthesized letters.
# dot.raw is the last "letter", so let's suppose if that one exists, all
# others have been generated as well
letters: letters_en letters_ru

letters_en: letters/en/dot.raw
letters_ru: letters/ru/dot.raw

# English letters
letters/en/dot.raw:
	cd letters/en; cat letters.lst | gawk -f ../../scripts/generate_letters tts=../../scripts/tts/en rate=0.5 default_pitch=1.0 high_pitch=1.25 volume=2.0 frequency=16000

# Russian letters
letters/ru/dot.raw:
	cd letters/ru; cat letters.lst | gawk -f ../../scripts/generate_letters tts=../../scripts/tts/ru rate=0.0 default_pitch=0.2 high_pitch=0.4 volume=0.8 frequency=10000

clean:
	cd src/freephone; $(MAKE) clean
	cd src/rawplay; $(MAKE) clean
	cd src/tones; $(MAKE) clean
	cd src/multilingual; $(MAKE) clean
